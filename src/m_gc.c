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

#define M_LOG_TAG "gc"

#include <m_log.h>
#include <m_gc.h>
#include <m_list.h>

/**New allocated buffer.*/
typedef struct {
	uint8_t *begin; /**< Buffer begin pointer.*/
	uint8_t *end;   /**< Buffer end pointer.*/
	uint8_t *curr;  /**< Current allocated pointer.*/
} M_GCNewBuf;

typedef struct M_GCObjStub_s M_GCObjStub;
typedef struct M_GCObjPool_s M_GCObjPool;
typedef struct M_GCObjCell_s M_GCObjCell;
typedef struct M_GCOldStub_s M_GCOldStub;
typedef struct M_GCOldBuf_s  M_GCOldBuf;
typedef struct M_GCBigBuf_s  M_GCBigBuf;

/**GC object pools stub.*/
struct M_GCObjStub_s {
	M_GCObjType  type;         /**< Object type.*/
	M_SList      usable_pools; /**< Pools have empty cells.*/
	M_SList      full_pools;   /**< Fulled pools.*/
	uint32_t     cell_size;    /**< Object size.*/
	uint32_t     cell_num;     /**< Cell number in one pool.*/
};

/**GC object pool.*/
struct M_GCObjPool_s {
	uint32_t    *bitmap;       /**< Bitmap of the cells.*/
	M_SList      free_cells;   /**< Free cells list.*/
	uint8_t     *begin;        /**< The first cell's pointer.*/
};

/**GC cell to hold object.*/
struct M_GCObjCell_s {
	M_SList      slist;        /**< Single linked list node.*/
};

/**GC old data buffer stub.*/
struct M_GCOldStub_s {
	M_List       buf_list;     /**< Buffer list.*/
	M_List       big_buf_list; /**< Bit buffer list.*/
};

/**Old data buffer.*/
struct M_GCOldBuf_s {
	uint8_t     *begin;        /**< The buffer.*/
};

/**Big data buffer.*/
struct M_GCBigBuf_s {
	M_List      *node;         /**< List node.*/
	uint32_t     size;         /**< Buffer size.*/
};

pthread_mutex_t m_gc_lock;

static uint32_t gc_page_shift;
static uint32_t gc_page_size;
static uint32_t gc_page_mask;
static uint32_t gc_new_buf_size;
static uint32_t gc_old_buf_size;

static M_GCNewBuf  gc_new_buf1, gc_new_buf2;
static M_GCNewBuf *gc_new_from_buf = &gc_new_buf1;
static M_GCNewBuf *gc_new_to_buf = &gc_new_buf2;

static const M_GCObjInfo*
gc_objs[] = {
};

static size_t
gc_size_align (size_t size)
{
	return (size + sizeof(uintptr_t) - 1) & ~(sizeof(uintptr_t) - 1);
}

static const M_GCObjInfo*
gc_get_obj_info (M_GCObjType type)
{
	assert((type >= 0) && (type < M_N_ELEMENT(gc_objs)));

	return gc_objs[type];
}

static void*
gc_alloc_obj (const M_GCObjInfo *oi)
{
}

static void*
gc_alloc_buf (size_t size, uint32_t flags)
{
	if (flags & M_GC_BUF_FL_OLD) {
	} else {
	}
}

static void*
gc_realloc_buf (void *ptr, size_t old_size, size_t new_size, uint32_t flags)
{
}

static void
gc_free (void *ptr, size_t size, uint32_t flags)
{
}

void
m_gc_startup (void)
{
	pthread_mutex_init(&m_gc_lock, NULL);
}

void
m_gc_shutdown (void)
{
	pthread_mutex_destroy(&m_gc_lock);
}

void*
m_gc_alloc_obj (M_GCObjType type)
{
	const M_GCObjInfo *oi;
	void *ptr;

	oi = gc_get_obj_info(type);

	pthread_mutex_lock(&m_gc_lock);

	ptr = gc_alloc_obj(oi);

	pthread_mutex_unlock(&m_gc_lock);

	return ptr;
}

void*
m_gc_alloc_buf (size_t size, uint32_t flags)
{
	void *ptr;

	size = gc_size_align(size);

	pthread_mutex_lock(&m_gc_lock);

	ptr = gc_alloc_buf(size, flags);

	pthread_mutex_unlock(&m_gc_lock);

	return ptr;
}

void*
m_gc_realloc_buf (void *ptr, size_t old_size, size_t new_size,
			uint32_t flags)
{
	if (!ptr || !old_size)
		return m_gc_alloc_buf(new_size, flags);

	if (!new_size) {
		m_gc_free_buf(ptr, old_size, flags);
		return NULL;
	}

	if (new_size <= old_size)
		return ptr;

	old_size = gc_size_align(old_size);
	new_size = gc_size_align(new_size);

	pthread_mutex_lock(&m_gc_lock);

	ptr = gc_realloc_buf(ptr, old_size, new_size, flags);

	pthread_mutex_unlock(&m_gc_lock);

	return ptr;
}

void
m_gc_free_buf (void *ptr, uint32_t size, uint32_t flags)
{
	if (!ptr || !size)
		return;

	size = gc_size_align(size);

	pthread_mutex_lock(&m_gc_lock);

	gc_free(ptr, size, flags);

	pthread_mutex_unlock(&m_gc_lock);
}

