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

#define M_LOG_TAG "startup"

#include <m_log.h>
#include <m_startup.h>
#include <m_log.h>

static pthread_once_t once = PTHREAD_ONCE_INIT;

static void
shutdown (void)
{
	M_INFO("ming shutdown");

	m_log_shutdown();
}

static void
startup (void)
{
	m_log_startup();

	M_INFO("ming startup");

	atexit(shutdown);
}

void
m_startup (void)
{
	pthread_once(&once, startup);
}

