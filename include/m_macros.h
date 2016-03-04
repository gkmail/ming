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
 * Basic macro definitions.
 */

#ifndef _M_MACROS_H_
#define _M_MACROS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <m_arch.h>

/**Begin a statement like macro definition.*/
#define M_STMT_BEGIN do {
/**End of a statement like macro definition.*/
#define M_STMT_END   } while (0)

/**Calculate the maximum value of 2 values.*/
#define M_MAX(a, b) ((a) > (b) ? (a) : (b))

/**Calculate the minimum value of 2 values.*/
#define M_MIN(a, b) ((a) < (b) ? (a) : (b))

/**
 * Calculate the clamp value.
 * When v < min, return min.
 * When v > max, return max.
 * Otherwise return v.
 */
#define M_CLAMP(v, min, max) \
	((v) < (min) ? (min) : ((v) > (max) ? (max) : (v)))

/**Calculate the maximum value of 3 values.*/
#define M_MAX3(a, b, c) M_MAX(a, M_MAX(b, c))

/**Calculate the minimum value of 3 values.*/
#define M_MIN3(a, b, c) M_MIN(a, M_MIN(b, c))

/**Calculate the absolute value.*/
#define M_ABS(a) ((a) >= 0 ? (a) : -(a))

/**Convert a pointer to size type.*/
#define M_PTR_TO_SIZE(p) ((size_t)(p))

/**Convert a size type to pointer.*/
#define M_SIZE_TO_PTR(s) ((void*)(size_t)(s))

/**Get the element count of an array.*/
#define M_N_ELEMENT(a) (sizeof(a) / sizeof((a)[0]))

/**
 * Get the offset in bytes of a member in a structure type.
 * \a type is the structure type.
 * \a member is the structure member's name.
 */
#define M_OFFSET_OF(type, member) \
	((size_t)(uint8_t*)(&((type*)0)->member))

/**
 * Get the container structure pointer from a member pointer.
 * \a ptr is the member's pointer.
 * \a type is the structure type.
 * \a member is the structure member's name.
 */
#define M_CONTAINER_OF(ptr, type, member)\
	((type*)(((uint8_t*)(ptr)) - M_OFFSET_OF(type, member)))

/**Exchange 2 values.*/
#define M_EXCHANGE(a, b)\
	M_STMT_BEGIN\
		typeof(a) _tmp = a;\
		a = b;\
		b = _tmp;\
	M_STMT_END

#ifdef __cplusplus
}
#endif

#endif
