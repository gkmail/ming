/******************************************************************************
 * Ming: a free scripting language running platform                           *
 *----------------------------------------------------------------------------*
 * Copyright (C) 2016  L+#= +0=1 <gkmail@sina.com>                            *
 *                                                                            *
 * This program is free software: you can redistribute it and/or modify       *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation, either version 3 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * This program is distributed in the hope that it will be useful,            *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License          *
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.      *
 *****************************************************************************/

#define M_LOG_TAG "gc_obj"

#include <m_log.h>
#include <m_thread.h>
#include <m_malloc.h>
#include "m_gc_internal.h"

#ifndef M_GC_GRAY_STACK_SIZE
	#define M_GC_GRAY_STACK_SIZE 256
#endif

#ifndef M_GC_BEGIN_SIZE
	#define M_GC_BEGIN_SIZE (128*1024)
#endif

/**New borned object has not any pointer in it.*/
#define GC_NB_FL_NO_PTR 1

/*Mark flags.*/
#define GC_MARK_UNUSED 0
#define GC_MARK_WHITE  1
#define GC_MARK_GRAY   2
#define GC_MARK_BLACK  3

/**GC status.*/
typedef enum {
	GC_STATUS_IDLE,      /**< GC is not running.*/
	GC_STATUS_MARK_ROOT, /**< Mark root objects.*/
	GC_STATUS_MARK_OBJ,  /**< Mark normal objects.*/
	GC_STATUS_SWEEP,     /**< Sweep unused objects.*/
	GC_STATUS_COMPACT    /**< Compact memory buffer.*/
} GCStatus;

/**Gray object stack.*/
typedef struct {
	void **stack;        /**< The bottom of the stack.*/
	void **end;          /**< The end of the stack buffer.*/
	void **top;          /**< The top of the stack.*/
} GCGrayStack;

/**Cell pool size in bytes.*/
static size_t gc_cell_pool_size;
/**Cell pool address mask.*/
static size_t gc_cell_pool_mask;
/**GC begin size.*/
static size_t gc_begin_size;
/**Gray object stack.*/
static GCGrayStack gc_gray_stack;
/**GC status.*/
static GCStatus  gc_status;
/**Thread the GC process in running on it.*/
static M_Thread *gc_thread;
/**Some gray objects have not been scanned.*/
static M_Bool    gc_need_scan_gray;

#include "m_gc_funcs.c"
#include "m_gc_descrs.c"

/**Object pools stubs array.*/
static M_GCPoolStub gc_obj_stubs[M_GC_OBJ_COUNT];

/**Get the pool contains the object.*/
static inline M_GCCellPool*
gc_obj_get_pool (void *ptr)
{
	return (M_GCCellPool*)(M_PTR_TO_SIZE(ptr) & gc_cell_pool_mask);
}

/**Get the object description from its type.*/
static inline const M_GCObjDescr*
gc_obj_get_descr (M_GCObjType type)
{
	assert((type >= 0) && (type < M_GC_OBJ_COUNT));

	return &gc_obj_descrs[type];
}

/**Get the object's index in the pool.*/
static inline int
gc_obj_get_id (const M_GCObjDescr *descr, M_GCCellPool *pool, void *ptr)
{
	size_t diff = ((uint8_t*)ptr) - pool->begin;

	return diff / descr->size;
}

/**Set the bitmap value of the object.*/
static inline void
gc_obj_set_bitmap (M_GCCellPool *pool, int id, int flags)
{
	uint32_t *bmp = pool->bitmap;
	int n, b, mask;

	n = id >> 4;
	b = (id & 0xF) << 1;
	mask = 3 << b;

	bmp[n] &= ~mask;
	bmp[n] |= flags << b;
}

/**Get the bitmap value of the object.*/
static inline int
gc_obj_get_bitmap (M_GCCellPool *pool, int id)
{
	uint32_t *bmp = pool->bitmap;
	int n, b;

	n = id >> 4;
	b = (id & 0xF) << 1;

	return (bmp[n] >> b) & 3;
}

/**Allocate a new cell pool.*/
static M_GCCellPool*
gc_alloc_pool (M_GCObjType type, const M_GCObjDescr *descr, M_GCPoolStub *stub)
{
	M_GCCellPool *pool;
	M_GCCell *cell;
	M_SList **pnode;
	uint8_t *ptr;
	int num;

	if (!(pool = gc_mmap(gc_cell_pool_size, 0)))
		return NULL;

	pool->type = type;

	ptr = (uint8_t*)(pool + 1);
	pool->bitmap = (uint32_t*)ptr;

	memset(pool->bitmap, 0, stub->bitmap_size);

	ptr += stub->bitmap_size;
	pool->begin = ptr;

	num   = stub->cell_num;
	pnode = &pool->free_cells.next;

	while (num --) {
		cell = (M_GCCell*)ptr;
		*pnode = &cell->node;
		pnode  = &cell->node.next;

		ptr += descr->size;
	}

	*pnode = NULL;

	m_slist_push(&stub->usable_pools, &pool->node);

	return pool;
}

/**Free the cell pool.*/
static void
gc_free_pool (M_GCCellPool *pool)
{
	gc_munmap(pool, gc_cell_pool_size);
}

/**
 * Push the object to gray stack.
 * \param[in] ptr The pointer of the object.
 * \retval M_TRUE The object is pushed.
 * \retval M_FALSE The stack is full.
 */
static inline M_Bool
gc_push_gray_stack (void *ptr)
{
	if (gc_gray_stack.top == gc_gray_stack.end) {
		gc_need_scan_gray = M_TRUE;
		return M_FALSE;
	} else {
		*gc_gray_stack.top ++ = ptr;
		return M_TRUE;
	}
}

/**Mark the object with color.*/
static inline void
gc_mark_with_color (void *ptr, int set_flags)
{
	const M_GCObjDescr *descr;
	M_GCCellPool *pool;
	int id, flags;

	pool  = gc_obj_get_pool(ptr);
	descr = gc_obj_get_descr(pool->type);
	id    = gc_obj_get_id(descr, pool, ptr);

	flags = gc_obj_get_bitmap(pool, id);
	if (flags == GC_MARK_WHITE) {
		if ((set_flags == GC_MARK_BLACK) || !(descr->flags & M_GC_OBJ_FL_PTR))
			flags = GC_MARK_BLACK;

		gc_obj_set_bitmap(pool, id, flags);

		if (flags == GC_MARK_GRAY)
			gc_push_gray_stack(ptr);
	}
}

/**Mark the object as gray.*/
static inline void
gc_mark (void *ptr)
{
	gc_mark_with_color(ptr, GC_MARK_GRAY);
}

/**Mark threads' new borned object stack.*/
static void
gc_mark_nb_stacks (void)
{
	M_Thread *th;

	m_list_foreach_value(th, &m_thread_list, node) {
		uintptr_t *pptr, *pend;
		void *ptr;

		pptr = th->nb_stack;
		pend = pptr + th->nb_top;

		while (pptr < pend) {
			if (*pptr & GC_NB_FL_NO_PTR) {
				ptr = M_SIZE_TO_PTR(*pptr & ~GC_NB_FL_NO_PTR);
				gc_mark_with_color(ptr, GC_MARK_BLACK);
			} else {
				ptr = M_SIZE_TO_PTR(*pptr);
				gc_mark(ptr);
			}
			pptr ++;
		}
	}
}

/**Mark root objects in hash table.*/
static void
gc_mark_root_hash (void)
{
	M_GCRootNode *rn;
	uint32_t pos;

	m_hash_foreach_value(rn, pos, &gc_root_hash, node) {
		gc_mark(rn->ptr);
	}
}

/**Mark root objcets.*/
static void
gc_mark_root (void)
{
	M_DEBUG("mark root objects");

	gc_need_scan_gray = M_FALSE;
	gc_mark_nb_stacks();
	gc_mark_root_hash();
}

/**Scan gray objects in the pool.*/
static M_Bool
gc_scan_pool_gray (M_GCCellPool *pool)
{
	const M_GCObjDescr *descr;
	M_GCPoolStub *stub;
	uint32_t *bmp, *bend;
	int id;

	descr = gc_obj_get_descr(pool->type);
	stub  = &gc_obj_stubs[pool->type];
	bmp   = pool->bitmap;
	bend  = (uint32_t*)(((uint8_t*)bmp) + stub->bitmap_size);
	id    = 0;

	while (bmp < bend) {
		if (*bmp) {
			uint32_t flags = *bmp;
			uint8_t *ptr = pool->begin + id * descr->size;
			int mark;

			while (1) {
				mark = flags & 3;
				if (mark == GC_MARK_GRAY) {
					if (!gc_push_gray_stack(ptr))
						return M_FALSE;
				}

				flags >>= 2;
				if (!flags)
					break;

				ptr += descr->size;
			}
		}
		bmp ++;
		id += 16;
	}

	return M_TRUE;
}

/**Scan gray objects.*/
static void
gc_scan_gray (void)
{
	M_GCPoolStub *stub;
	M_GCCellPool *pool;
	M_GCObjType type;

	assert(gc_need_scan_gray);

	M_DEBUG("scan gray objects");

	gc_need_scan_gray = M_FALSE;

	for (type = 0; type < M_GC_OBJ_COUNT; type ++) {
		stub = &gc_obj_stubs[type];

		m_slist_foreach_value(pool, &stub->full_pools, node) {
			if (!gc_scan_pool_gray(pool))
				return;
		}

		m_slist_foreach_value(pool, &stub->usable_pools, node) {
			if (!gc_scan_pool_gray(pool))
				return;
		}
	}
}

/**Mark objects.*/
static M_Bool
gc_mark_objs (void)
{
	M_GCCellPool *pool;
	void *ptr;

	if (gc_gray_stack.top == gc_gray_stack.stack) {
		if (!gc_need_scan_gray)
			return M_TRUE;

		gc_scan_gray();
	}

	assert(gc_gray_stack.top > gc_gray_stack.stack);

	M_DEBUG("mark objects");

	do {
		ptr = *(--gc_gray_stack.top);
		pool  = gc_obj_get_pool(ptr);
		gc_scan(pool->type, ptr);
	} while (gc_gray_stack.top > gc_gray_stack.stack);

	return !gc_need_scan_gray;
}

/**Sweep unused object in pool.*/
static void
gc_sweep_pool (const M_GCObjDescr *descr, M_GCPoolStub *stub, M_GCCellPool *pool)
{
	M_Bool have_black = M_FALSE;
	uint32_t *bmp, *bend;
	int id;

	bmp   = pool->bitmap;
	bend  = (uint32_t*)(((uint8_t*)bmp) + stub->bitmap_size);
	id    = 0;

	while (bmp < bend) {
		if (*bmp) {
			uint32_t flags = *bmp;
			uint8_t *ptr = pool->begin + id * descr->size;
			int mark, shift = 0;

			while (1) {
				mark = flags & 3;
				if (mark == GC_MARK_WHITE) {
					/*Collect unused object.*/
					M_GCCell *cell = (M_GCCell*)ptr;

					gc_final(pool->type, ptr);

					m_slist_push(&pool->free_cells, &cell->node);
					*bmp &= ~(3 << shift);

					/*Update total allocated size.*/
					gc_allocated_size -= descr->size;
				} else if (mark == GC_MARK_BLACK) {
					/*Inuse object.*/
					*bmp &= ~(3 << shift);
					*bmp |= (GC_MARK_WHITE << shift);
					have_black = M_TRUE;
				} else if (mark == GC_MARK_GRAY) {
					assert(0);
				}

				flags >>= 2;
				if (!flags)
					break;

				ptr += descr->size;
				shift += 2;
			}
		}
		bmp ++;
		id += 16;
	}

	if (have_black) {
		/*Add the pool to the stub's list.*/
		if (m_slist_empty(&pool->free_cells)) {
			m_slist_push(&stub->full_pools, &pool->node);
		} else {
			m_slist_push(&stub->usable_pools, &pool->node);
		}
	} else {
		/*Free the empty pool.*/
		gc_free_pool(pool);
	}
}

/**Sweep unused objects.*/
static void
gc_sweep (void)
{
	const M_GCObjDescr *descr;
	M_GCPoolStub *stub;
	M_GCCellPool *pool;
	M_GCObjType type;
	size_t old_size = gc_allocated_size;

	M_DEBUG("sweep objects");

	for (type = 0; type < M_GC_OBJ_COUNT; type ++) {
		M_SList full_pools, usable_pools;
		M_GCCellPool *npool;

		descr = gc_obj_get_descr(type);
		stub  = &gc_obj_stubs[type];

		full_pools   = stub->full_pools;
		usable_pools = stub->usable_pools;

		m_slist_init(&stub->full_pools);
		m_slist_init(&stub->usable_pools);

		m_slist_foreach_value_safe(pool, npool, &full_pools, node) {
			gc_sweep_pool(descr, stub, pool);
		}

		m_slist_foreach_value_safe(pool, npool, &usable_pools, node) {
			gc_sweep_pool(descr, stub, pool);
		}
	}

	M_DEBUG("collect %d bytes", old_size - gc_allocated_size);

	gc_last_allocated_size = gc_allocated_size;
}

/**Collection.*/
static M_Bool
gc_do_collect (uint32_t flags)
{
	M_Bool b;

	switch (gc_status) {
		case GC_STATUS_IDLE:
			gc_status = GC_STATUS_MARK_ROOT;
		case GC_STATUS_MARK_ROOT:
			if (!(flags & M_GC_COLLECT_FL_CLEAR)) {
				gc_mark_root();
				gc_status = GC_STATUS_MARK_OBJ;
				if (flags & M_GC_COLLECT_FL_INCREMENT)
					return M_FALSE;
			} else {
				gc_status = GC_STATUS_MARK_OBJ;
			}
		case GC_STATUS_MARK_OBJ:
			if (!(flags & M_GC_COLLECT_FL_CLEAR)) {
				do {
					b = gc_mark_objs();
					if (b)
						gc_status = GC_STATUS_SWEEP;
					if (flags & M_GC_COLLECT_FL_INCREMENT)
						return M_FALSE;
				} while (!b);
			} else {
				gc_status = GC_STATUS_SWEEP;
			}
		case GC_STATUS_SWEEP:
			gc_sweep();
			gc_status = GC_STATUS_COMPACT;
			if (flags & M_GC_COLLECT_FL_INCREMENT)
				return M_FALSE;
		case GC_STATUS_COMPACT:
			gc_status = GC_STATUS_IDLE;
	}

	return M_TRUE;
}

/**Start GC process.*/
static M_Bool
gc_collect_objs (uint32_t flags)
{
	M_Thread *th;
	M_Bool r;

	th = m_thread_self();

	if (!gc_thread) {
		gc_thread = th;

		/*Pause all threads.*/
		m_thread_pause_all();

		/*Unlock the GC lock. It is safe because only one thread is running.*/
		pthread_mutex_unlock(&m_gc_lock);

		/*Collection.*/
		M_DEBUG("gc begin");

		gc_do_collect(flags);

		M_DEBUG("gc end");

		/*Relock GC lock*/
		pthread_mutex_lock(&m_gc_lock);

		/*Resume all threads.*/
		m_thread_resume_all();

		gc_thread = NULL;
	} else {
		if (th != gc_thread) {
			/*GC is running in another thread, just wait it.*/
			m_thread_check_nl();
			r = (gc_status == GC_STATUS_IDLE);
		} else {
			/*GC is running in current thread.*/
			r = M_FALSE;
		}
	}

	return r;
}

static void*
gc_alloc_obj (M_GCObjType type, size_t *oid)
{
	const M_GCObjDescr *od;
	M_GCPoolStub *stub;
	M_GCCellPool *pool;
	M_GCCell *cell;
	M_Thread *th;
	int id;
	uintptr_t addr;

	assert((type >= 0) && (type < M_GC_OBJ_COUNT));

	od   = gc_obj_get_descr(type);
	stub = &gc_obj_stubs[type];

	/*Get the usable pool.*/
	if (m_slist_empty(&stub->usable_pools)) {
		pool = gc_alloc_pool(type, od, stub);
		if (!pool)
			return NULL;
	} else {
		pool = m_node_value(stub->usable_pools.next, M_GCCellPool, node);
	}

	/*Resize the thread's new borned stack.*/
	th = m_thread_self();
	if (th->nb_top == th->nb_size) {
		uintptr_t *nbuf;
		uint32_t nsize;

		/*Resize the new borned object stack.*/
		nsize = M_MAX(th->nb_size * 2, 32);
		nbuf  = m_gc_realloc_buf(th->nb_stack,
					th->nb_size * sizeof(uintptr_t),
					nsize * sizeof(uintptr_t),
					M_GC_NBSTK_FLAGS);
		m_assert_alloc(nbuf);

		th->nb_stack = nbuf;
		th->nb_size  = nsize;

		M_DEBUG("resize new borned stack to %d", nsize);
	}

	/*Get a free cell.*/
	cell = m_node_value(m_slist_pop(&pool->free_cells), M_GCCell, node);

	/*Move the pool to full list if it has not any free cells.*/
	if (m_slist_empty(&pool->free_cells)) {
		pool = m_node_value(m_slist_pop(&stub->usable_pools),
					M_GCCellPool, node);
		m_slist_push(&stub->full_pools, &pool->node);
	}

	/*Push the new object and mark it is not initialized.*/
	*oid = th->nb_top;
	addr = M_PTR_TO_SIZE(cell) | GC_NB_FL_NO_PTR;

	th->nb_stack[th->nb_top ++] = addr;

	/*Set mask to white.*/
	id = gc_obj_get_id(od, pool, cell);
	gc_obj_set_bitmap(pool, id, GC_MARK_WHITE);

	/*Update total allocated size.*/
	gc_allocated_size += od->size;

	/*Test if collection is needed.*/
	if ((gc_allocated_size >= gc_begin_size) &&
				(gc_allocated_size * 2 > gc_last_allocated_size * 3)) {
		gc_collect_objs(0);
	}

	return cell;
}

void
gc_obj_startup (void)
{
	const M_GCObjDescr *descr;
	M_GCPoolStub *stub;
	M_GCObjType type;
	char *val;
	size_t size;
	long int n;

	gc_status = GC_STATUS_IDLE;
	gc_thread = NULL;

	/*Get begin size.*/
	gc_begin_size = M_GC_BEGIN_SIZE;

	val = getenv("M_GC_BEGIN_SIZE");
	if (val) {
		n = strtol(val, NULL, 0);
		if ((n != LONG_MAX) && (n > 0)) {
			gc_begin_size = n;
		}
	}
	M_INFO("gc begin size:%d", gc_begin_size);

	/*Get pool size.*/
	gc_cell_pool_size = M_PAGE_SIZE;

	val = getenv("M_GC_CELL_POOL_SIZE");
	if (val) {
		n = strtol(val, NULL, 0);
		if ((n != LONG_MAX) && (n > gc_cell_pool_size)) {
			size_t v = 1;

			do {
				if (v >= n)
					break;
			} while (v <<= 1);

			if (n != v) {
				M_ERROR("illegal cell pool size. it must be power of 2.");
			} else {
				gc_cell_pool_size = n;
			}
		}
	}
	M_INFO("gc cell pool size:%d", gc_cell_pool_size);

	gc_cell_pool_mask = ~(gc_cell_pool_size - 1);

	/*Initialize gray object stack.*/
	size = M_GC_GRAY_STACK_SIZE;

	val = getenv("M_GC_GRAY_STACK_SIZE");
	if (val) {
		n = strtol(val, NULL, 0);
		if ((n != LONG_MAX) && (n > 0))
			size = n;
	}
	M_INFO("gc gray stack size:%d", size);

	gc_gray_stack.stack = M_NEW(void*, size);
	m_assert_alloc(gc_gray_stack.stack);
	gc_gray_stack.top = gc_gray_stack.stack;
	gc_gray_stack.end = gc_gray_stack.stack + size;

	/*Stubs initialize.*/
	for (type = 0; type < M_GC_OBJ_COUNT; type ++) {
		size_t size, num, bs;

		descr = &gc_obj_descrs[type];
		stub  = &gc_obj_stubs[type];

		m_slist_init(&stub->usable_pools);
		m_slist_init(&stub->full_pools);

		size = gc_cell_pool_size - sizeof(M_GCCellPool);

		/*Calculate the cell number and bitmap size.*/
		num  = (size * 8) / (descr->size * 8 + 2);

		while (1) {
			bs = M_ALIGN_UP(num * 2, sizeof(uintptr_t) * 8) / 8;

			if (num * descr->size + bs <= size)
				break;

			num --;
		}

		stub->cell_num    = num;
		stub->bitmap_size = bs;

		M_DEBUG("type:%d cell size:%d cell num:%d bitmap size:%d",
					type, descr->size, num, bs);
	}
}

void
gc_obj_shutdown (void)
{
	/*Collect all the objects.*/
	M_DEBUG("clear objcets");
	gc_do_collect(M_GC_COLLECT_FL_CLEAR);

	/*Free gray stack.*/
	m_free(gc_gray_stack.stack);
}

void*
m_gc_alloc_obj (M_GCObjType type, size_t *oid)
{
	void *ptr;

	assert(oid);

	pthread_mutex_lock(&m_gc_lock);

	ptr = gc_alloc_obj(type, oid);

	pthread_mutex_unlock(&m_gc_lock);

	return ptr;
}

void
m_gc_run (uint32_t flags)
{
	pthread_mutex_lock(&m_gc_lock);

	gc_collect_objs(flags);

	pthread_mutex_unlock(&m_gc_lock);
}

