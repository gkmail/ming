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

#include <m_object.h>
#include <m_closure.h>
#include <m_frame.h>
#include <m_string.h>
#include <m_array.h>

#define M_GC_PTR_FLAGS M_GC_OBJ_FL_PTR
#define M_GC_PTR_SIZE  sizeof(void*)

static inline void
gc_ptr_scan (void *ptr)
{
}

#define gc_ptr_final NULL

#define M_GC_DOUBLE_FLAGS 0
#define M_GC_DOUBLE_SIZE  sizeof(double)
#define gc_double_scan  NULL
#define gc_double_final NULL

#define M_GC_STRING_FLAGS M_GC_OBJ_FL_PTR
#define M_GC_STRING_SIZE  sizeof(M_String)
static inline void
gc_string_scan (void *ptr)
{
}

static inline void
gc_string_final (void *ptr)
{
}

#define M_GC_OBJECT_FLAGS M_GC_OBJ_FL_PTR
#define M_GC_OBJECT_SIZE  sizeof(M_Object)
static inline void
gc_object_scan (void *ptr)
{
}

static inline void
gc_object_final (void *ptr)
{
}

#define M_GC_CLOSURE_FLAGS M_GC_OBJ_FL_PTR
#define M_GC_CLOSURE_SIZE  sizeof(M_Closure)
static inline void
gc_closure_scan (void *ptr)
{
}

static inline void
gc_closure_final (void *ptr)
{
}

#define M_GC_ARRAY_FLAGS M_GC_OBJ_FL_PTR
#define M_GC_ARRAY_SIZE  sizeof(M_Array)
static inline void
gc_array_scan (void *ptr)
{
}

static inline void
gc_array_final (void *ptr)
{
}

#define M_GC_FRAME_FLAGS M_GC_OBJ_FL_PTR
#define M_GC_FRAME_SIZE  sizeof(M_Frame)
static inline void
gc_frame_scan (void *ptr)
{
}

static inline void
gc_frame_final (void *ptr)
{
}

