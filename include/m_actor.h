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

#ifndef _M_ACTOR_H_
#define _M_ACTOR_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "m_types.h"
#include "m_list.h"
#include "m_thread.h"

typedef struct M_Stack_s M_Stack;

struct M_Stack_s {
	M_Stack   *bottom;
	M_SList    slist;
	M_Frame   *frame;
	uint16_t   ip;
	uint16_t   sp;
};

struct M_Actor_s {
	M_SList    stack;
};

/**
 * Get the current actor.
 * \return The current actor.
 */
static inline M_Actor*
m_actor_self (void)
{
	return m_thread_self()->actor;
}

#ifdef __cplusplus
}
#endif

#endif
