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
#include "m_gc_internal.h"

pthread_mutex_t m_gc_lock;

size_t gc_allocated_size;
size_t gc_last_allocated_size;

void
m_gc_startup (void)
{
	pthread_mutex_init(&m_gc_lock, NULL);
	
	gc_allocated_size = 0;
	gc_last_allocated_size = 0;

	gc_obj_startup();
	gc_root_hash_startup();
}

void
m_gc_shutdown (void)
{
	gc_obj_shutdown();
	gc_root_hash_shutdown();

	pthread_mutex_destroy(&m_gc_lock);
}


