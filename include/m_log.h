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

/**
 * \file
 * Log message output function.
 */

#ifndef _M_LOG_H_
#define _M_LOG_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "m_types.h"

/**Log output level.*/
typedef enum {
	M_LOG_LEVEL_ALL,    /**< Output all message.*/
	M_LOG_LEVEL_DEBUG,  /**< Debug message.*/
	M_LOG_LEVEL_INFO,   /**< Information.*/
	M_LOG_LEVEL_WARNING,/**< Warning message.*/
	M_LOG_LEVEL_ERROR,  /**< Error message.*/
	M_LOG_LEVEL_FATAL,  /**< Fatal error message.*/
	M_LOG_LEVEL_NONE    /**< Do not output any message.*/
} M_LogLevel;

/** \cond */
extern void m_log_startup (void);
extern void m_log_shutdown (void);
/** \endcond */

/**
 * Output log message.
 * \param level Output level.
 * \param[in] tag Message tag.
 * \param[in] file File which generate this meesage.
 * \param[in] func Function which generate this message.
 * \param line Line number where invoke "m_log".
 * \param[in] fmt Output format string.
 */
extern void m_log (M_LogLevel level,
		const char *tag,
		const char *file,
		const char *func,
		int line,
		const char *fmt,
		...);

/**Output debug message.*/
#define M_DEBUG(a...)\
	m_log(M_LOG_LEVEL_DEBUG, M_LOG_TAG, __FILE__, __FUNCTION__, __LINE__, a)
/**Output information.*/
#define M_INFO(a...)\
	m_log(M_LOG_LEVEL_INFO, M_LOG_TAG, __FILE__, __FUNCTION__, __LINE__, a)
/**Output warning message.*/
#define M_WARNING(a...)\
	m_log(M_LOG_LEVEL_WARNING, M_LOG_TAG, __FILE__, __FUNCTION__, __LINE__, a)
/**Output error message.*/
#define M_ERROR(a...)\
	m_log(M_LOG_LEVEL_ERROR, M_LOG_TAG, __FILE__, __FUNCTION__, __LINE__, a)
/**Output fatal error message.*/
#define M_FATAL(a...)\
	m_log(M_LOG_LEVEL_FATAL, M_LOG_TAG, __FILE__, __FUNCTION__, __LINE__, a)

#ifdef __cplusplus
}
#endif

#endif
