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
 * Atomic operation.
 */

#ifndef _M_ATOMIC_H_
#define _M_ATOMIC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "m_types.h"

/**Atomic read an integer.*/
#define m_atomic_get_int(p)\
	({\
	 __sync_synchronize ();\
	 (int)*();\
	 })
/**Atomic read a 32 bits integer.*/
#define m_atomic_get_int32(p)\
	({\
	 __sync_synchronize ();\
	 (int32_t)*(p);\
	 })
/**Atomic read a 64 bits integer.*/
#define m_atomic_get_int64(p)\
	({\
	 __sync_synchronize ();\
	 (int64_t)*(p);\
	 })
/**Atomic read a pointer.*/
#define m_atomic_get_ptr(p)\
	({\
	 __sync_synchronize ();\
	 (void*)*(p);\
	 })

/**Atomic write an integer.*/
#define m_atomic_set_int(p, v)\
	({\
	 *(p) = (v);\
	 __sync_synchronize ();\
	 })
/**Atomic write a 32 bits integer.*/
#define m_atomic_set_int32(p, v)\
	({\
	 *(p) = (v);\
	 __sync_synchronize ();\
	 })
/**Atomic write a 64 bits integer.*/
#define m_atomic_set_int64(p, v)\
	({\
	 *(p) = (v);\
	 __sync_synchronize ();\
	 })
/**Atomic write a pointer.*/
#define m_atomic_set_ptr(p, v)\
	({\
	 *(p) = M_SIZE_TO_PTR(v);\
	 __sync_synchronize ();\
	 })

/**Atomic increase an integer and return the origin value.*/
#define m_atomic_int_inc(p)   __sync_fetch_and_add(p, 1)
/**Atomic increase a 32 bits integer and return the origin value.*/
#define m_atomic_int32_inc(p) __sync_fetch_and_add(p, 1)
/**Atomic increase a 64 bits integer and return the origin value.*/
#define m_atomic_int64_inc(p) __sync_fetch_and_add(p, 1)
/**Atomic increase a pointer and return the origin value.*/
#define m_atomic_ptr_inc(p)   __sync_fetch_and_add(p, 1)

/**Atomic decrease an integer and return the origin value.*/
#define m_atomic_int_dec(p)   __sync_fetch_and_sub(p, 1)
/**Atomic decrease a 32 bits integer and return the origin value.*/
#define m_atomic_int32_dec(p) __sync_fetch_and_sub(p, 1)
/**Atomic decrease a 64 bits integer and return the origin value.*/
#define m_atomic_int64_dec(p) __sync_fetch_and_sub(p, 1)
/**Atomic decrease a pointer and return the origin value.*/
#define m_atomic_ptr_dec(p)   __sync_fetch_and_sub(p, 1)

/**Atomic add operation to an integer and return the origin value.*/
#define m_atomic_int_add(p, v)   __sync_fetch_and_add(p, v)
/**Atomic add operation to a 32 bits integer and return the origin value.*/
#define m_atomic_int32_add(p, v) __sync_fetch_and_add(p, v)
/**Atomic add operation to a 64 bits integer and return the origin value.*/
#define m_atomic_int64_add(p, v) __sync_fetch_and_add(p, v)
/**Atomic add operation to a pointer and return the origin value.*/
#define m_atomic_ptr_add(p, v)   __sync_fetch_and_add(p, v)

/**Atomic sub operation to an integer and return the origin value.*/
#define m_atomic_int_sub(p, v)   __sync_fetch_and_sub(p, v)
/**Atomic sub operation to a 32 bits integer and return the origin value.*/
#define m_atomic_int32_sub(p, v) __sync_fetch_and_sub(p, v)
/**Atomic sub operation to a 64 bits integer and return the origin value.*/
#define m_atomic_int64_sub(p, v) __sync_fetch_and_sub(p, v)
/**Atomic sub operation to a pointer and return the origin value.*/
#define m_atomic_ptr_sub(p, v)   __sync_fetch_and_sub(p, v)

/**Atomic and operation to an integer and return the origin value.*/
#define m_atomic_int_and(p, v)   __sync_fetch_and_and(p, v)
/**Atomic and operation to a 32 bits integer and return the origin value.*/
#define m_atomic_int32_and(p, v) __sync_fetch_and_and(p, v)
/**Atomic and operation to a 64 bits integer and return the origin value.*/
#define m_atomic_int64_and(p, v) __sync_fetch_and_and(p, v)
/**Atomic and operation to a pointer and return the origin value.*/
#define m_atomic_ptr_and(p, v)   __sync_fetch_and_and(p, v)

/**Atomic or operation to an integer and return the origin value.*/
#define m_atomic_int_or(p, v)   __sync_fetch_and_or(p, v)
/**Atomic or operation to a 32 bits integer and return the origin value.*/
#define m_atomic_int32_or(p, v) __sync_fetch_and_or(p, v)
/**Atomic or operation to a 64 bits integer and return the origin value.*/
#define m_atomic_int64_or(p, v) __sync_fetch_and_or(p, v)
/**Atomic or operation to a pointer and return the origin value.*/
#define m_atomic_ptr_or(p, v)   __sync_fetch_and_or(p, v)

/**Atomic xor operation to an integer and return the origin value.*/
#define m_atomic_int_xor(p, v)   __sync_fetch_and_xor(p, v)
/**Atomic xor operation to a 32 bits integer and return the origin value.*/
#define m_atomic_int32_xor(p, v) __sync_fetch_and_xor(p, v)
/**Atomic xor operation to a 64 bits integer and return the origin value.*/
#define m_atomic_int64_xor(p, v) __sync_fetch_and_xor(p, v)
/**Atomic xor operation to a pointer and return the origin value.*/
#define m_atomic_ptr_xor(p, v)   __sync_fetch_and_xor(p, v)

/**Atomic compare and swap operation to an integer.*/
#define m_atomic_int_cas(p, o, n)   __sync_bool_compare_and_swap(p, o, n)
/**Atomic compare and swap operation to a 32 bits integer.*/
#define m_atomic_int32_cas(p, o, n) __sync_bool_compare_and_swap(p, o, n)
/**Atomic compare and swap operation to a 64 bits integer.*/
#define m_atomic_int64_cas(p, o, n) __sync_bool_compare_and_swap(p, o, n)
/**Atomic compare and swap operation to a pointer.*/
#define m_atomic_ptr_cas(p, o, n)   __sync_bool_compare_and_swap(p, o, n)

#ifdef __cplusplus
}
#endif

#endif
