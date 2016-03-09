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

#ifndef _M_FRAME_H_
#define _M_FRAME_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "m_types.h"

#define M_FRAME_FL_IN_STACK 1

/**Value frame.*/
struct M_Frame_s {
	uint16_t   flags;   /**< Flags.*/
	uint16_t   nv;      /**< Number of values.*/
	M_Value   *v;       /**< Values.*/
	M_Closure *closure; /**< The closure.*/
};

#ifdef __cplusplus
}
#endif

#endif
