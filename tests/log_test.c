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

#define M_LOG_TAG "log_test"

#include <ming.h>

static void
log_test (void)
{
	M_DEBUG("debug");
	M_INFO("info");
	M_WARNING("warning");
	M_ERROR("error");
	M_FATAL("fatal");
}

static void*
thread_entry (void *arg)
{
	int id = M_PTR_TO_SIZE(arg);
	int i;

	for (i = 0; i < 100; i ++) {
		M_DEBUG("from thread %d", id);
	}

	return NULL;
}

static void
multithread_test (void)
{
	int count = 32;
	pthread_t threads[count];
	int i;

	for (i = 0; i < count; i ++) {
		pthread_create(&threads[i], NULL, thread_entry, M_SIZE_TO_PTR(i));
	}

	for (i = 0; i < count; i ++) {
		pthread_join(threads[i], NULL);
	}
}

int
main (int argc, char **argv)
{
	m_startup();

	log_test();
	multithread_test();

	return 0;
}

