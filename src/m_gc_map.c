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

#define M_LOG_TAG "gc_map"

#include <m_log.h>
#include "m_gc_internal.h"

/** System map function. */
static inline void*
sys_mmap(void *ptr, size_t size, uint32_t flags)
{
	int prot;
	void *r;

	prot = PROT_READ|PROT_WRITE;

	if (flags & M_GC_MAP_FL_EXEC)
		prot |= PROT_EXEC;

	r = mmap(ptr, size, prot, MAP_PRIVATE|MAP_ANON, -1, 0);

	if (r != MAP_FAILED) {
		/*M_DEBUG("map %dB at %p", size, r);*/
	} else {
		M_ERROR("map %dB failed", size);
		return NULL;
	}

	return r;
}

static inline void
sys_munmap (void *ptr, size_t size)
{
	munmap(ptr, size);
	/*M_DEBUG("unmap %dB at %p", size, ptr);*/
}

void*
gc_mmap (size_t size, uint32_t flags)
{
	uint8_t *pu8;
	uintptr_t diff;
	void *ptr;

	size = (size + M_PAGE_MASK) & ~M_PAGE_MASK;

	/*Try to allocate an aligned pool*/
	pu8 = (uint8_t*)sys_mmap(NULL, size, flags);
	if (pu8 == MAP_FAILED)
		return NULL;

	if (!(M_PTR_TO_SIZE(pu8) & M_PAGE_MASK))
		return pu8;

	diff = M_PAGE_SIZE - (M_PTR_TO_SIZE(pu8) & M_PAGE_MASK);
	if (sys_mmap(pu8 + size, diff, flags)) {
		sys_munmap(pu8, diff);
		return pu8 + diff;
	}

	diff = M_PTR_TO_SIZE(pu8) & M_PAGE_MASK;
	if (sys_mmap(pu8 - diff, diff, flags)) {
		sys_munmap(pu8 + size - diff, diff);
		return pu8 - diff;
	}

	sys_munmap(pu8, size);

	/*Allocate a big buffer and unmap the unaligned part*/
	if (!(pu8 = (uint8_t*)sys_mmap(NULL, size + M_PAGE_SIZE - 1, flags)))
		return NULL;

	diff = M_PTR_TO_SIZE(pu8) & M_PAGE_MASK;
	if (diff) {
		diff = M_PAGE_SIZE - diff;
		sys_munmap(pu8, diff);

		ptr = (void*)(pu8 + diff);

		pu8 += diff + size;
		diff = M_PAGE_SIZE - 1 - diff;
	} else {
		ptr = (void*)pu8;

		pu8 += size;
		diff = M_PAGE_SIZE - 1;
	}

	sys_munmap(pu8, diff);

	return ptr;
}

void
gc_munmap (void *ptr, size_t size)
{
	sys_munmap(ptr, size);
}

