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

/**The object contains pointer.*/
#define M_GC_OBJ_FL_PTR   1
/**The object can be marked as white directly.*/
#define M_GC_OBJ_FL_WHITE 2

/**The buffer must be allocated in old zone.*/
#define M_GC_BUF_FL_OLD   1
/**Executable code buffer.*/
#define M_GC_BUF_FL_EXE   2
/**The buffer contains pointer.*/
#define M_GC_BUF_FL_PTR   4

/**GC managed object type.*/
typedef enum M_GCObjType_e M_GCObjType;

/**Pointer mark function.*/
typedef void (*M_GCMarkFunc)(void *ptr, M_GCObjType type, uint32_t size);

/**GC data type information.*/
typedef struct {
	uint32_t flags;  /**< Flags.*/
	uint32_t size;   /**< Size in bytes.*/
	/**Mark the pointer in the data buffer.*/
	void (*mark) (void *ptr, uint32_t size, M_GCMarkFunc func);
	/**Data type finalize function.*/
	void (*final) (void *ptr, uint32_t size);
} M_GCObjInfo;

/**GC managed data type.*/
enum M_GCObjType_e {
	M_GC_BUF = -1,     /**< Buffer, not an object.*/
	M_GC_OBJ_PTR,      /**< Pointer pointed to an object or closure.*/
	M_GC_OBJ_DOUBLE,   /**< Double precision number.*/
	M_GC_OBJ_STRING,   /**< String.*/
	M_GC_OBJ_OBJECT,   /**< Object.*/
	M_GC_OBJ_FUNCTION, /**< Function.*/
	M_GC_OBJ_CLOSURE,  /**< Closure.*/
	M_GC_OBJ_ARRAY,    /**< Array.*/
	M_GC_OBJ_FRAME     /**< Value frame.*/
};

/** \cond */
extern pthread_mutex_t m_gc_lock;

extern void m_gc_startup (void);
extern void m_gc_shutdown (void);
/** \endcond */

/**
 * Allocate a new object managed by GC.
 * \param type GC object type.
 * \return The pointer of the new buffer.
 * \retval NULL On error.
 */
extern void* m_gc_alloc_obj (M_GCObjType type);

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

#ifdef __cplusplus
}
#endif

#endif
