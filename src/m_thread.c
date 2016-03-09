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
#include <m_malloc.h>

#define M_GC_THREAD_FLAGS (M_GC_BUF_FL_OLD | M_GC_BUF_FL_PTR)
#define M_GC_NBSTK_FLAGS  (M_GC_BUF_FL_OLD | M_GC_BUF_FL_PTR)

pthread_key_t m_thread_key;
uint32_t      m_thread_num;
uint32_t      m_paused_thread_num;

static M_Bool thread_pause_flag;
static pthread_cond_t thread_pause_cond;
static pthread_cond_t thread_resume_cond;

static void
thread_key_destructor (void *ptr)
{
	M_Thread *th = (M_Thread*)ptr;

	/*Free thread data.*/
	if (th) {
		if (th->nb_stack) {
			m_gc_free_buf(th, sizeof(void*) * th->nb_size, M_GC_NBSTK_FLAGS);
		}

		m_gc_free_buf(th, sizeof(M_Thread), M_GC_THREAD_FLAGS);
	}

	/*Decrease thread number.*/
	pthread_mutex_lock(&m_gc_lock);

	m_thread_num --;

	pthread_mutex_unlock(&m_gc_lock);
}

static M_Thread*
thread_create (void)
{
	M_Thread *th;

	/*Allocate thread data.*/
	th = m_gc_alloc_buf(sizeof(M_Thread), M_GC_THREAD_FLAGS);
	m_assert_alloc(th);

	th->actor    = NULL;
	th->nb_stack = NULL;
	th->nb_size  = 0;
	th->nb_top   = 0;

	pthread_setspecific(m_thread_key, th);

	/*Increase thread number.*/
	pthread_mutex_lock(&m_gc_lock);

	m_thread_num ++;

	pthread_mutex_unlock(&m_gc_lock);

	return th;
}

void
m_thread_startup (void)
{
	M_Thread *mth;

	pthread_key_create(&m_thread_key, thread_key_destructor);

	pthread_cond_init(&thread_pause_cond, NULL);
	pthread_cond_init(&thread_resume_cond, NULL);

	m_thread_num = 0;
	m_paused_thread_num = 0;

	thread_pause_flag = M_FALSE;

	mth = thread_create();
}

void
m_thread_shutdown (void)
{
	M_Thread *mth;

	mth = m_thread_self();
	if (mth) {
		thread_key_destructor(mth);
	}

	pthread_key_delete(m_thread_key);
	pthread_cond_destroy(&thread_pause_cond);
	pthread_cond_destroy(&thread_resume_cond);
}

void
m_thread_pause_all (void)
{
	thread_pause_flag = M_TRUE;

	while (m_thread_num != m_paused_thread_num) {
		pthread_cond_wait(&thread_pause_cond, &m_gc_lock);
	}
}

void
m_thread_resume_all (void)
{
	thread_pause_flag = M_FALSE;

	pthread_cond_broadcast(&thread_resume_cond);
}

void
m_thread_leave (void)
{
	pthread_mutex_lock(&m_gc_lock);

	m_paused_thread_num ++;

	pthread_mutex_unlock(&m_gc_lock);

	pthread_cond_signal(&thread_pause_cond);
}

void
m_thread_enter (void)
{
	pthread_mutex_lock(&m_gc_lock);

	while (thread_pause_flag) {
		pthread_cond_wait(&thread_resume_cond, &m_gc_lock);
	}

	m_paused_thread_num --;

	pthread_mutex_unlock(&m_gc_lock);
}

void
m_thread_check (void)
{
	pthread_mutex_lock(&m_gc_lock);

	if (thread_pause_flag) {
		m_paused_thread_num ++;
		pthread_cond_signal(&thread_pause_cond);

		while (thread_pause_flag) {
			pthread_cond_wait(&thread_resume_cond, &m_gc_lock);
		}

		m_paused_thread_num --;
	}

	pthread_mutex_unlock(&m_gc_lock);
}

