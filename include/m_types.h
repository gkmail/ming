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
 * Basic types definitions.
 */

#ifndef _M_TYPES_H_
#define _M_TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "m_arch.h"
#include "m_macros.h"

/**Boolean value.*/
typedef uint8_t M_Bool;

/**Boolean value true.*/
#define M_TRUE  1
/**Boolean value false.*/
#define M_FALSE 0

/**Function result value.*/
typedef int M_Result;

/**Function result: success.*/
#define M_OK          1
/**Function result: no error but do nothing.*/
#define M_NONE        0
/**Function result: failed.*/
#define M_FAILED     -1
/**Function result: not enough memory.*/
#define M_ERR_NO_MEM -2
/**Function result: value type error.*/
#define M_ERR_TYPE   -3

/**String.*/
typedef struct M_String_s   M_String;
/**Interned string.*/
typedef M_String*           M_Quark;
/**Array.*/
typedef struct M_Array_s    M_Array;
/**Object.*/
typedef struct M_Object_s   M_Object;
/**Function.*/
typedef struct M_Function_s M_Function;
/**Module.*/
typedef struct M_Module_s   M_Module;
/**Closure.*/
typedef struct M_Closure_s  M_Closure;
/**Value stack frame.*/
typedef struct M_Frame_s    M_Frame;
/**Actor.*/
typedef struct M_Actor_s    M_Actor;
/**General value.*/
typedef uintptr_t           M_Value;
/**Thread related data.*/
typedef struct M_Thread_s   M_Thread;

#ifdef __cplusplus
}
#endif

#endif
