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

#define M_LOG_TAG "gctest"

#include <ming.h>

static void
gc_test (void)
{
#define PTR_COUNT (1024*1024)
	size_t level;
	static void *ptrs[PTR_COUNT];
	double *pd;
	size_t id;
	int i;
	int count = PTR_COUNT;

	M_INFO("gc test begin");

	level = m_gc_get_nb_level();

	for (i = 0; i < count; i ++) {
		pd = m_gc_alloc_obj(M_GC_OBJ_DOUBLE, &id);
		*pd = i;
		m_gc_add_obj(id);
	}

	m_gc_run(0);

	m_gc_set_nb_level(level);

	m_gc_run(0);

	M_INFO("gc test end");
}

static void*
thread_entry (void *arg)
{
#define LOOP_COUNT  100
#define ALLOC_COUNT 1024
	int id = M_PTR_TO_SIZE(arg);
	size_t level, obj_id;
	int i, l;
	double *pd;

	m_thread_enter();

	for (l = 0; l < LOOP_COUNT; l ++) {
		level = m_gc_get_nb_level();

		for (i = 0; i < ALLOC_COUNT; i ++) {
			pd = m_gc_alloc_obj(M_GC_OBJ_DOUBLE, &obj_id);
			*pd = i;
			m_gc_add_obj(obj_id);
		}

		m_gc_run(0);

		m_gc_set_nb_level(level);

		m_gc_run(0);
	}

	m_thread_leave();

	return NULL;
}

static void
multithread_test (void)
{
#define THREAD_COUNT 64
	pthread_t th[THREAD_COUNT];
	int i;

	M_INFO("multithread test begin");

	for (i = 0; i < THREAD_COUNT; i ++) {
		pthread_create(&th[i], NULL, thread_entry, M_SIZE_TO_PTR(i));
	}

	for (i = 0; i < THREAD_COUNT; i ++) {
		m_thread_leave();

		pthread_join(th[i], NULL);

		m_thread_enter();
	}

	M_INFO("multithread test end");
}

int
main (int argc, char **argv)
{
	m_startup();

	gc_test();
	multithread_test();

	return 0;
}

