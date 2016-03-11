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

#define M_LOG_TAG "ming"

#include <m_log.h>
#include <m_thread.h>
#include <m_gc.h>
#include <m_list.h>
#include <m_malloc.h>

#define M_GC_THREAD_FLAGS (M_GC_BUF_FL_PERMANENT | M_GC_BUF_FL_PTR)
#define M_GC_ATH_FLAGS    M_GC_BUF_FL_PERMANENT

/**Created actor thread.*/
typedef struct {
	M_SList   node;
	pthread_t thread;
} ActorThread;

M_List          m_thread_list;
pthread_key_t   m_thread_key;
pthread_mutex_t m_thread_lock;
uint32_t        m_thread_num;
uint32_t        m_paused_thread_num;

static M_SList actor_thread_list;
static M_Bool  thread_pause_flag;
static M_Bool  thread_exit_flag;
static pthread_cond_t thread_pause_cond;
static pthread_cond_t thread_resume_cond;
static pthread_cond_t thread_active_cond;

/**Thread data desctructor.*/
static void
thread_key_destructor (void *ptr)
{
	M_Thread *th = (M_Thread*)ptr;

	if (th) {
		/*Decrease thread number.*/
		pthread_mutex_lock(&m_gc_lock);

		m_list_remove(&th->node);

		m_thread_num --;

		if(th->flags & M_THREAD_FL_PAUSED)
			m_paused_thread_num --;

		pthread_mutex_unlock(&m_gc_lock);

		/*Free thread data.*/
		if (th->nb_stack) {
			m_gc_free_buf(th->nb_stack, sizeof(uintptr_t) * th->nb_size,
						M_GC_NBSTK_FLAGS);
		}

		m_gc_free_buf(th, sizeof(M_Thread), M_GC_THREAD_FLAGS);
	}
}

/**Create a new thread.*/
static M_Thread*
thread_register (void)
{
	M_Thread *th;

	/*Allocate thread data.*/
	th = m_gc_alloc_buf(sizeof(M_Thread), M_GC_THREAD_FLAGS);
	m_assert_alloc(th);

	th->actor    = NULL;
	th->nb_stack = NULL;
	th->nb_size  = 0;
	th->nb_top   = 0;
	th->flags    = 0;

	pthread_setspecific(m_thread_key, th);

	pthread_mutex_lock(&m_gc_lock);

	/*Increase thread number.*/
	m_thread_num ++;

	/*Insert the thread to the list.*/
	m_list_append(&m_thread_list, &th->node);

	pthread_mutex_unlock(&m_gc_lock);

	M_DEBUG("register thread %p", th);

	return th;
}

/**Thread entry function.*/
static void*
thread_entry (void *arg)
{
	pthread_mutex_lock(&m_thread_lock);

	while (!thread_exit_flag) {
		pthread_cond_wait(&thread_active_cond, &m_thread_lock);
	}

	pthread_mutex_unlock(&m_thread_lock);
}

void
m_thread_startup (void)
{
	M_Thread *mth;

	pthread_key_create(&m_thread_key, thread_key_destructor);

	pthread_mutex_init(&m_thread_lock, NULL);

	pthread_cond_init(&thread_pause_cond, NULL);
	pthread_cond_init(&thread_resume_cond, NULL);
	pthread_cond_init(&thread_active_cond, NULL);

	m_thread_num = 0;
	m_paused_thread_num = 0;

	thread_pause_flag = M_FALSE;
	thread_exit_flag  = M_FALSE;

	m_list_init(&m_thread_list);
	m_slist_init(&actor_thread_list);

	mth = thread_register();
}

void
m_thread_shutdown (void)
{
	ActorThread *node, *nnode;
	M_Thread *mth;

	/*Stop all the threads.*/
	pthread_mutex_lock(&m_thread_lock);

	thread_exit_flag = M_TRUE;

	pthread_mutex_unlock(&m_thread_lock);

	pthread_cond_broadcast(&thread_active_cond);

	m_slist_foreach_value_safe(node, nnode, &actor_thread_list, node) {
		pthread_join(node->thread, NULL);
		m_gc_free_buf(node, sizeof(ActorThread), M_GC_ATH_FLAGS);
	}

	/*Free thread data.*/
	mth = m_thread_self();
	if (mth) {
		thread_key_destructor(mth);
	}

	/*Release resource.*/
	pthread_key_delete(m_thread_key);
	pthread_mutex_destroy(&m_thread_lock);
	pthread_cond_destroy(&thread_pause_cond);
	pthread_cond_destroy(&thread_resume_cond);
	pthread_cond_destroy(&thread_active_cond);
}

void
m_thread_pause_all (void)
{
	M_Thread *th = m_thread_self();

	assert(!(th->flags & M_THREAD_FL_PAUSED));

	thread_pause_flag = M_TRUE;

	m_paused_thread_num ++;
	th->flags |= M_THREAD_FL_PAUSED;

	M_DEBUG("pause %d %d", m_thread_num, m_paused_thread_num);

	while (m_thread_num != m_paused_thread_num) {
		pthread_cond_wait(&thread_pause_cond, &m_gc_lock);
		M_DEBUG("pause %d %d", m_thread_num, m_paused_thread_num);
	}
}

void
m_thread_resume_all (void)
{
	M_Thread *th = m_thread_self();

	assert(th->flags & M_THREAD_FL_PAUSED);

	thread_pause_flag = M_FALSE;

	m_paused_thread_num --;
	th->flags &= ~M_THREAD_FL_PAUSED;

	pthread_cond_broadcast(&thread_resume_cond);
}

void
m_thread_leave (void)
{
	M_Thread *th = m_thread_self();

	assert(!(th->flags & M_THREAD_FL_PAUSED));

	pthread_mutex_lock(&m_gc_lock);

	m_paused_thread_num ++;
	th->flags |= M_THREAD_FL_PAUSED;

	pthread_mutex_unlock(&m_gc_lock);

	pthread_cond_signal(&thread_pause_cond);
}

void
m_thread_enter (void)
{
	M_Thread *th;

	pthread_mutex_lock(&m_gc_lock);

	while (thread_pause_flag) {
		pthread_cond_wait(&thread_resume_cond, &m_gc_lock);
	}

	th = pthread_getspecific(m_thread_key);
	if (!th) {
		/* Invoke m_thread_enter in this thread for the first time.
		 * Create a new thread data.*/
		pthread_mutex_unlock(&m_gc_lock);

		thread_register();
	} else {
		/* Reenter. */
		assert(th->flags & M_THREAD_FL_PAUSED);

		m_paused_thread_num --;
		th->flags &= ~M_THREAD_FL_PAUSED;

		pthread_mutex_unlock(&m_gc_lock);
	}
}

void
m_thread_check_nl (void)
{
	M_Thread *th = m_thread_self();

	assert(!(th->flags & M_THREAD_FL_PAUSED));

	if (thread_pause_flag) {
		m_paused_thread_num ++;
		th->flags |= M_THREAD_FL_PAUSED;

		pthread_cond_signal(&thread_pause_cond);

		while (thread_pause_flag) {
			pthread_cond_wait(&thread_resume_cond, &m_gc_lock);
		}

		m_paused_thread_num --;
		th->flags &= ~M_THREAD_FL_PAUSED;
	}
}

void
m_thread_check (void)
{
	pthread_mutex_lock(&m_gc_lock);

	m_thread_check_nl();

	pthread_mutex_unlock(&m_gc_lock);
}

void
m_thread_create (void)
{
	ActorThread *node;
	int r;

	if (thread_exit_flag)
		return;

	node = m_gc_alloc_buf(sizeof(ActorThread), M_GC_ATH_FLAGS);
	if (!node)
		return;

	/*Create the thread.*/
	r = pthread_create(&node->thread, NULL, thread_entry, NULL);
	if (r) {
		m_gc_free_buf(node, sizeof(ActorThread), M_GC_ATH_FLAGS);
		return;
	}

	/*Add it to the thread list.*/
	m_slist_push(&actor_thread_list, &node->node);
}

