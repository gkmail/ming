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

/**
 * \file
 * Garbage collector.
 */

#ifndef _M_GC_H_
#define _M_GC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "m_types.h"
#include "m_thread.h"

/**The object contains pointer.*/
#define M_GC_OBJ_FL_PTR   1

/**The buffer contains pointer.*/
#define M_GC_BUF_FL_PTR        1
/**The buffer is a permanent buffer.*/
#define M_GC_BUF_FL_PERMANENT  2
/**Executable code buffer.*/
#define M_GC_BUF_FL_EXECUTABLE 4

/**GC managed object type.*/
typedef enum M_GCObjType_e M_GCObjType;

/**GC data type description.*/
typedef struct {
	uint32_t flags;  /**< Flags.*/
	uint32_t size;   /**< Size in bytes.*/
	/**Scan the pointer in the data buffer.*/
	void (*scan) (void *ptr);
	/**Data type finalize function.*/
	void (*final) (void *ptr);
} M_GCObjDescr;

/**GC managed data type.*/
enum M_GCObjType_e {
	M_GC_BUF = -1,     /**< Buffer, not an object.*/
	M_GC_OBJ_PTR,      /**< Pointer pointed to an object or closure.*/
	M_GC_OBJ_DOUBLE,   /**< Double precision number.*/
	M_GC_OBJ_STRING,   /**< String.*/
	M_GC_OBJ_OBJECT,   /**< Object.*/
	M_GC_OBJ_CLOSURE,  /**< Closure.*/
	M_GC_OBJ_ARRAY,    /**< Array.*/
	M_GC_OBJ_FRAME,    /**< Value frame.*/
	M_GC_OBJ_COUNT     /**< Count of the object types.*/
};

/** \cond */
extern pthread_mutex_t m_gc_lock;

extern void m_gc_startup (void);
extern void m_gc_shutdown (void);
/** \endcond */

/**
 * Get the current thread's new borned stack level.
 * \return The current thread's new borned stack's level value.
 */
static inline size_t
m_gc_get_nb_level (void)
{
	M_Thread *th;

	th = m_thread_self();

	return th->nb_top;
}

/**
 * Restore the current thread's new borned stack level.
 * \param level The new level value of the new borned stack.
 */
static inline size_t
m_gc_set_nb_level (size_t level)
{
	M_Thread *th;

	th = m_thread_self();

	assert(level <= th->nb_top);

	th->nb_top = level;
}

/**
 * Allocate a new object managed by GC.
 * \param type GC object type.
 * \param[out] oid Return the object's index.
 * After allocation, the object's buffer is not initialized.
 * You should do something to initialize the object.
 * And invoke "m_gc_add_obj" to make the object is valid for GC.
 * "oid" is used by "m_gc_add_obj".
 * \return The pointer of the new buffer.
 * \retval NULL On error.
 */
extern void* m_gc_alloc_obj (M_GCObjType type, size_t *oid);

/**
 * Set a new allocated object is valid for GC.
 * \param oid The object's index returned from "m_gc_alloc_obj".
 */
static inline void
m_gc_add_obj (size_t oid)
{
	M_Thread *th = m_thread_self();

	assert(oid < th->nb_top);

	/*Clear not initialize flag.*/
	th->nb_stack[oid] &= ~3;
}

/**
 * Allocate a new buffer managed by GC.
 * \param size The buffer size in bytes.
 * \param flags Allocate flags.
 * \return The pointer of the new buffer.
 * \retval NULL On error.
 */
extern void* m_gc_alloc_buf (size_t size, uint32_t flags);

/**
 * Resize a buffer managed by GC.
 * \param[in] ptr The old buffer's pointer.
 * \param old_size The buffer's old size in bytes.
 * \param new_size The buffer's new size in bytes.
 * \param flags Allocate flags.
 * \return The pointer of the new buffer.
 * \retval NULL On error.
 */
extern void* m_gc_realloc_buf (void *ptr, size_t old_size,
			size_t new_size, uint32_t flags);

/**
 * Free an unused buffer managed by GC.
 * \param[in] ptr The pointer of the buffer.
 * \param size The buffer size in bytes.
 * \param flags Allocate flags.
 */
extern void  m_gc_free_buf (void *ptr, uint32_t size, uint32_t flags);

/**
 * Make the object as root object.
 * \param ptr The root object's pointer.
 */
extern void  m_gc_add_root (void *ptr);

/**
 * Remove an root object.
 * \param ptr The root object's pointer.
 */
extern void  m_gc_remove_root (void *ptr);

/**
 * Start GC process.
 * \param flags GC running flags.
 */
extern void  m_gc_run (uint32_t flags);

#ifdef __cplusplus
}
#endif

#endif
