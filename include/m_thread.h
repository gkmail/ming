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

#ifndef _M_THREAD_H_
#define _M_THREAD_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "m_types.h"
#include "m_list.h"

/**The thread is paused.*/
#define M_THREAD_FL_PAUSED 1

/**Thread related data.*/
struct M_Thread_s {
	M_List     node;     /**< List node.*/
	M_Actor   *actor;    /**< Current running actor in this thread.*/
	uintptr_t *nb_stack; /**< New borned object stack.*/
	uint32_t   nb_size;  /**< New borned object stack size.*/
	uint32_t   nb_top;   /**< Top of the new borned object stack.*/
	uint32_t   flags;    /**< The thread's flags.*/
};

/** \cond */
#define M_GC_NBSTK_FLAGS  (M_GC_BUF_FL_PERMANENT | M_GC_BUF_FL_PTR)

extern M_List          m_thread_list;
extern pthread_key_t   m_thread_key;
extern pthread_mutex_t m_thread_lock;
extern uint32_t        m_thread_num;
extern uint32_t        m_paused_thread_num;

extern void m_thread_startup (void);
extern void m_thread_shutdown (void);
extern void m_thread_check_nl (void);
/** \endcond */

/**
 * Get the current thread data.
 * \return The current thread's data.
 */
static inline M_Thread*
m_thread_self (void)
{
	M_Thread *th;

	th = (M_Thread*)pthread_getspecific(m_thread_key);
	assert(th);

	return th;
}

/**
 * Pause all threads.
 */
extern void m_thread_pause_all (void);

/**
 * Resume all threads.
 */
extern void m_thread_resume_all (void);

/**
 * Leave the ming environment to run the native code.
 */
extern void  m_thread_leave (void);

/**
 * Enter the ming environment.
 */
extern void  m_thread_enter (void);

/**
 * Check if the GC process need to be started.
 * And pause the current thread until the GC process end when GC is running.
 */
extern void  m_thread_check (void);

/**
 * Create a new thread to run actors.
 */
extern void  m_thread_create (void);

#ifdef __cplusplus
}
#endif

#endif
