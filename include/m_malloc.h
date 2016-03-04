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
 * Memory allocate and free functions.
 */

#ifndef _M_MALLOC_H_
#define _M_MALLOC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "m_types.h"

/**
 * Allocate a new buffer.
 * \param size The buffer's size i bytes.
 * \return The new buffer's pointer.
 * \retval NULL Not enough memory.
 */
static inline void*
m_malloc (size_t size)
{
	return malloc(size);
}

/**
 * Resize the buffer's size.
 * \param[in] ptr The old buffer's pointer.
 * If \a ptr  == NULL, means allocate a new buffer.
 * \param size New buffer size in bytes.
 * If \a size == 0, means free the buffer.
 * \return The new buffer's pointer.
 */
static inline void*
m_realloc (void *ptr, size_t size)
{
	return realloc(ptr, size);
}

/**
 * Free an unused buffer.
 * \param[in] ptr The buffer's pointer.
 */
static inline void
m_free (void *ptr)
{
	free(ptr);
}

/**
 * Allocate a new buffer and fill it with 0.
 * \param size The buffer's size in bytes.
 * \return The buffer's pointer.
 * \retval NULL When not enough memory left.
 */
static void*
m_malloc0 (size_t size)
{
	void *ptr = m_malloc(size);

	if (ptr) {
		memset(ptr, 0, size);
	}

	return ptr;
}

/**Allocate a structure array which type is \a type.*/
#define M_NEW(type, n)  m_malloc(sizeof(type) * (n))
/**Allocate a structure array which type is \a type and fill it with 0.*/
#define M_NEW0(type, n) m_malloc0(sizeof(type) * (n))
/**Resize a structure array which type is \a type.*/
#define M_RENEW(ptr, type, n) m_realloc(ptr, sizeof(type) * (n))

#ifdef __cplusplus
}
#endif

#endif
