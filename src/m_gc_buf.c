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

#define M_LOG_TAG "gc_buf"

#include <m_log.h>
#include <m_malloc.h>
#include "m_gc_internal.h"

/**Size alignment.*/
static size_t
gc_size_align (size_t size)
{
	return M_ALIGN_UP(size, sizeof(uintptr_t));
}

/**Allocate a new buffer.*/
static void*
gc_alloc_buf (size_t size, uint32_t flags)
{
}

/**Resize a buffer.*/
static void*
gc_realloc_buf (void *ptr, size_t old_size, size_t new_size, uint32_t flags)
{
}

/**Free a buffer.*/
static void
gc_free_buf (void *ptr, size_t size, uint32_t flags)
{
}

void
gc_buf_startup (void)
{
}

void
gc_buf_shutdown (void)
{
}

void*
m_gc_alloc_buf (size_t size, uint32_t flags)
{
	void *ptr;

	size = gc_size_align(size);

#if 0
	pthread_mutex_lock(&m_gc_lock);

	ptr = gc_alloc_buf(size, flags);

	pthread_mutex_unlock(&m_gc_lock);
#else
	ptr = m_malloc(size);
#endif
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

#if 0
	pthread_mutex_lock(&m_gc_lock);

	ptr = gc_realloc_buf(ptr, old_size, new_size, flags);

	pthread_mutex_unlock(&m_gc_lock);
#else
	ptr = m_realloc(ptr, new_size);
#endif
	return ptr;
}

void
m_gc_free_buf (void *ptr, uint32_t size, uint32_t flags)
{
	if (!ptr || !size)
		return;

	size = gc_size_align(size);

#if 0
	pthread_mutex_lock(&m_gc_lock);

	gc_free_buf(ptr, size, flags);

	pthread_mutex_unlock(&m_gc_lock);
#else
	m_free(ptr);
#endif
}

