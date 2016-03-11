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

#ifndef _M_GC_INTERNAL_H_
#define _M_GC_INTERNAL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <m_gc.h>
#include <m_list.h>
#include <m_hash.h>
#include <m_rbtree.h>

/**Pool stub.*/
typedef struct M_GCPoolStub_s  M_GCPoolStub;
/**Cell pool.*/
typedef struct M_GCCellPool_s  M_GCCellPool;
/**Cell.*/
typedef struct M_GCCell_s      M_GCCell;
/**Data buffer stub.*/
typedef struct M_GCBufStub_s   M_GCBufStub;
/**Data buffer pool.*/
typedef struct M_GCBufPool_s   M_GCBufPool;
/**Data buffer.*/
typedef struct M_GCBuf_s       M_GCBuf;
/**Big size data buffer.*/
typedef struct M_GCBigBuf_s    M_GCBigBuf;
/**Increament buffer pool.*/
typedef struct M_GCIncPool_s   M_GCIncPool;

/**Root object node.*/
typedef struct {
	M_HashNode node;        /**< Hash table node.*/
	void      *ptr;         /**< Pointer of the object.*/
	uint32_t   ref;         /**< Reference cunter.*/
} M_GCRootNode;

/**Pool stub.*/
struct M_GCPoolStub_s {
	M_SList   usable_pools; /**< Pools have empty cells.*/
	M_SList   full_pools;   /**< Pools without empty cells.*/
	size_t    cell_num;     /**< Cell number in one pool.*/
	size_t    bitmap_size;  /**< Bitmap size.*/
};

/**Cell pool.*/
struct M_GCCellPool_s {
	M_SList   node;         /**< List node.*/
	M_SList   free_cells;   /**< Free cell list.*/
	uint32_t *bitmap;       /**< Bitmap buffer.*/
	uint8_t  *begin;        /**< Beginning of the pool.*/
	M_GCObjType type;       /**< The object's type.*/
};

/**Fixed size cell.*/
struct M_GCCell_s {
	M_SList   node;         /**< Single linked list node.*/
};

/**Data buffer pool.*/
struct M_GCBufPool_s {
	M_List    node;         /**< List node.*/
	uint32_t *flags;        /**< Used flags.*/
};

/**Data buffer.*/
struct M_GCBuf_s {
	M_RBNode  addr_node;    /**< Address RB tree node.*/
	M_RBNode  size_node;    /**< Size RB tree node.*/
	size_t    size;         /**< Size of the buffer.*/
};

/**Big size buffer.*/
struct M_GCBigBuf_s {
	M_List    node;         /**< List node.*/
	size_t    size;         /**< Size of the buffer.*/
};

/**Data buffer stub.*/
struct M_GCBufStub_s {
	M_List    buf_pools;    /**< Buffer pools.*/
	M_List    big_list;     /**< Big size buffer list.*/
	/**< Small size buffer RB tree with size as its key.*/
	M_RBTree  small_size_rbt;
	/**< Small size buffer RB tree with address as its key.*/
	M_RBTree  small_addr_rbt;
	/**< Cell pool stubs.*/
	M_GCPoolStub stubs[sizeof(M_GCBuf) / sizeof(uintptr_t) - 1];
};

/**Increament buffer pool.*/
struct M_GCIncPool_s {
	uint8_t  *begin;        /**< Beginning of the pool.*/
	uint8_t  *end;          /**< End of the pool.*/
	uint8_t  *alloc;        /**< Current allocation position.*/
};

#ifndef M_PAGE_SIZE
	#define M_PAGE_SIZE 4096
#endif

#define M_PAGE_MASK (M_PAGE_SIZE - 1)

/**Map an executable buffer.*/
#define M_GC_MAP_FL_EXEC 1

/**Incremenet collection.*/
#define M_GC_COLLECT_FL_INCREMENT 1
/**Clear all objects.*/
#define M_GC_COLLECT_FL_CLEAR     2

/**Current allocated memory size in bytes.*/
extern size_t gc_allocated_size;
/**Memory size allocated after the last collection.*/
extern size_t gc_last_allocated_size;
/**Root object hash table.*/
extern M_Hash gc_root_hash;

/**
 * Map a page aligned buffer.
 * \param size Buffer size in bytes.
 * \param flags M_GC_MAP_FL_EXEC means buffer is executable.
 * \return The new mapped buffer.
 */
extern void*  gc_mmap (size_t size, uint32_t flags);

/**
 * Unmap a buffer.
 * \param[in] ptr The pointer of the buffer.
 * \param size The buffer's size in bytes.
 */
extern void   gc_munmap (void *ptr, size_t size);

/**
 * Object manager initialize.
 */
extern void   gc_obj_startup (void);

/**
 * Object manager release.
 */
extern void   gc_obj_shutdown (void);

/**
 * Buffer manager initialize.
 */
extern void   gc_buf_startup (void);

/**
 * Buffer manager release.
 */
extern void   gc_buf_shutdown (void);

/**
 * Root hash table initialize.
 */
extern void   gc_root_hash_startup (void);

/**
 * Root hash table release.
 */
extern void   gc_root_hash_shutdown (void);

#ifdef __cplusplus
}
#endif

#endif
