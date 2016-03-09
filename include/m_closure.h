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

#ifndef _M_CLOSURE_H_
#define _M_CLOSURE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "m_types.h"

#define M_CLOS_FL_INDEP 1

/**Closure.*/
struct M_Closure_s {
	M_Function *func;   /**< The function.*/
	M_Frame   **frames; /**< Value frames array.*/
	uint16_t    flags;  /**< Closure's flags.*/
	uint8_t     nframe; /**< The number of value frames.*/
};

#ifdef __cplusplus
}
#endif

#endif
