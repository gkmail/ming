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

/**Pointer offset.*/
typedef uint32_t M_GCPtrOffset;
/**Size.*/
typedef uint32_t M_GCSize;
/**GC managed object type.*/
typedef int      M_GCObjType;

/**GC managed object information.*/
typedef struct {
	M_GCSize             size; /**< Object size in bytes.*/
	const M_GCPtrOffset *ptrs; /**< Pointers offset in the object.*/
} M_GCObjInfo;

/**
 * Allocate a value buffer.
 * Value buffer is a buffer filled with M_Value.
 * GC should scan each value to find the pointer in it.
 * \param[in] size Value count in the buffer.
 * \return The new value buffer.
 */
extern M_Value* m_gc_alloc_vbuf (M_GCSize size);

/**
 * Allocate a data buffer.
 * Data buffer has not any pointer in it.
 * \param[in] Data buffer size in bytes.
 * \return The new data buffer.
 */
extern void*    m_gc_alloc_dbuf (M_GCSize size);

/**
 * Allocate a new object.
 * Object has some pointers in it.
 * GC should use pointer offst table in M_GCObjInfo to find the pointers.
 * \param[in] type The object type.
 * \return The pointer to new the object.
 */
extern void*    m_gc_alloc_obj (M_GCObjType type);

#ifdef __cplusplus
}
#endif

#endif
