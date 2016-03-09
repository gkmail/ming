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

#ifndef _M_VALUE_H_
#define _M_VALUE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "m_types.h"
#include "m_gc.h"
#include "m_malloc.h"

#if __SIZEOF_POINTER__ == 4
	#define M_VALUE_TYPE_SHIFT 2
	#define M_VALUE_DATA_MASK  0xFFFFFFFC
	#define M_VALUE_TRUE  0x7FFFFFFF
	#define M_VALUE_FALSE 0x80000003
	#define M_VALUE_INT_MAX 0x1FFFFFFE
	#define M_VALUE_INT_MIN 0x20000001
#else  /*__SIZEOF_POINTER__ == 8*/
	#define M_VALUE_TYPE_SHIFT 3
	#define M_VALUE_DATA_MASK  0xFFFFFFFFFFFFFFF8l
	#define M_VALUE_TYPE_BOOL 4
	#define M_VALUE_TRUE  0xC
	#define M_VALUE_FALSE 0x4
#endif /*__SIZEOF_POINTER__*/

#define M_VALUE_TYPE_MASK ((1 << M_VALUE_TYPE_SHIFT) - 1)
#define M_VALUE_TYPE_PTR      0
#define M_VALUE_TYPE_DOUBLE   1
#define M_VALUE_TYPE_STRING   2
#define M_VALUE_TYPE_INT      3

#define M_PTR_TYPE_MASK       3
#define M_PTR_TYPE_OBJECT     0
#define M_PTR_TYPE_CLOSURE    1
#define M_PTR_TYPE_ARRAY      2

/**
 * Check if the value is a boolean value.
 * \param v The value.
 * \retval M_TRUE The value is a boolean value.
 * \retval M_FALSE The value is not a boolean value.
 */
static inline M_Bool
m_value_is_bool (M_Value v)
{
	return (v == M_VALUE_TRUE) || (v == M_VALUE_FALSE);
}

/**
 * Check if the value is an integer number.
 * \param v The value.
 * \retval M_TRUE The value is an integer number.
 * \retval M_FALSE The value is not an integer number.
 */
static inline M_Bool
m_value_is_int (M_Value v)
{
#if __SIZEOF_POINTER__ == 4
	return ((v & M_VALUE_TYPE_MASK) == M_VALUE_TYPE_INT) &&
		!m_value_is_bool(v);
#else
	return (v & M_VALUE_TYPE_MASK) == M_VALUE_TYPE_INT;
#endif
}

/**
 * Check if the value is a double precision number.
 * \param v The value.
 * \retval M_TRUE The value is a double precision number.
 * \retval M_FALSE The value is not a double precision number.
 */
static inline M_Bool
m_value_is_double (M_Value v)
{
	return (v & M_VALUE_TYPE_MASK) == M_VALUE_TYPE_DOUBLE;
}

/**
 * Check if the value is a number(integer or double precision number).
 * \param v The value.
 * \retval M_TRUE The value is a number.
 * \retval M_FALSE The value is not a number.
 */
static inline M_Bool
m_value_is_number (M_Value v)
{
	return m_value_is_int(v) || m_value_is_double(v);
}

/**
 * Check if the value is a string.
 * \param v The value.
 * \retval M_TRUE The value is a string.
 * \retval M_FALSE The value is not a string.
 */
static inline M_Bool
m_value_is_string (M_Value v)
{
	return (v & M_VALUE_TYPE_MASK) == M_VALUE_TYPE_STRING;
}

/**
 * Check if the value is NULL.
 * \param v The value.
 * \retval M_TRUE The value is NULL.
 * \retval M_FALSE The value is not NULL.
 */
static inline M_Bool
m_value_is_null (M_Value v)
{
	void **ptr;

	if (!v)
		return M_TRUE;

	if ((v & M_VALUE_TYPE_MASK) != M_VALUE_TYPE_PTR)
		return M_FALSE;

	ptr = (void**)v;

	if (!*ptr)
		return M_TRUE;

	return M_FALSE;
}

/**
 * Check if the value is pointer.
 * \param v The value.
 * \retval M_TRUE The value is a pointer.
 * \retval M_FALSE The value is not a pointer.
 */
static inline M_Bool
m_value_is_ptr (M_Value v)
{
	void **ptr;

	if (!v)
		return M_FALSE;

	if ((v & M_VALUE_TYPE_MASK) != M_VALUE_TYPE_PTR)
		return M_FALSE;

	ptr = (void**)v;

	if (!*ptr)
		return M_FALSE;

	return M_TRUE;
}

/**
 * Check if the value is an object.
 * \param v The value.
 * \retval M_TRUE The value is an object.
 * \retval M_FALSE The value is not an object.
 */
static inline M_Bool
m_value_is_object (M_Value v)
{
	uintptr_t *ptr;

	if (!m_value_is_ptr(v))
		return M_FALSE;

	ptr = (uintptr_t*)v;
	return (*ptr & M_PTR_TYPE_MASK) == M_PTR_TYPE_OBJECT;
}

/**
 * Check if the value is a closure.
 * \param v The value.
 * \retval M_TRUE The value is a closure.
 * \retval M_FALSE The value is not a closure.
 */
static inline M_Bool
m_value_is_closure (M_Value v)
{
	uintptr_t *ptr;

	if (!m_value_is_ptr(v))
		return M_FALSE;

	ptr = (uintptr_t*)v;
	return (*ptr & M_PTR_TYPE_MASK) == M_PTR_TYPE_CLOSURE;
}

/**
 * Check if the value is an array.
 * \param v The value.
 * \retval M_TRUE The value is an array.
 * \retval M_FALSE The value is not an array.
 */
static inline M_Bool
m_value_is_array (M_Value v)
{
	uintptr_t *ptr;

	if (!m_value_is_ptr(v))
		return M_FALSE;

	ptr = (uintptr_t*)v;
	return (*ptr & M_PTR_TYPE_MASK) == M_PTR_TYPE_ARRAY;
}

/**
 * Get the boolean value from the value.
 * \param v The value.
 * \return The boolean value.
 */
static inline M_Bool
m_value_get_bool (M_Value v)
{
	assert(m_value_is_bool(v));

	return (v == M_VALUE_TRUE) ? M_TRUE : M_FALSE;
}

/**
 * Get the integer number from the value.
 * \param v The value.
 * \return The integer number.
 */
static inline int
m_value_get_int (M_Value v)
{
	assert(m_value_is_int(v));

	return ((int32_t)v) >> M_VALUE_TYPE_SHIFT;
}

/**
 * Get the double precision number from the value.
 * \param v The value.
 * \return The double precision number.
 */
static inline double
m_value_get_double (M_Value v)
{
	assert(m_value_is_double(v));

	return *(double*)(v & ~M_VALUE_TYPE_MASK);
}

/**
 * Get the number from the value.
 * \param v The value.
 * \return The number.
 */
static inline double
m_value_get_number (M_Value v)
{
	if (m_value_is_int(v)) {
		return (double)m_value_get_int(v);
	} else {
		assert(m_value_is_double(v));

		return m_value_get_double(v);
	}
}

/**
 * Get the string from the value.
 * \param v The value.
 * \return The string.
 */
static inline M_String*
m_value_get_string (M_Value v)
{
	assert(m_value_is_string(v));

	return (M_String*)(v & ~M_VALUE_TYPE_MASK);
}

/**
 * Get the pointer from the value.
 * \param v The value.
 * \return The pointer.
 */
static inline void*
m_value_get_ptr (M_Value v)
{
	uintptr_t ptr;

	assert(m_value_is_ptr(v));

	ptr = *(uintptr_t*)v;

	return (void*)(ptr & ~M_PTR_TYPE_MASK);
}

/**
 * Get the object from the value.
 * \param v The value.
 * \return The object.
 */
static inline M_Object*
m_value_get_object (M_Value v)
{
	assert(m_value_is_object(v));

	return (M_Object*) m_value_get_ptr(v);
}

/**
 * Get the closure from the value.
 * \param v The value.
 * \return The integer number.
 */
static inline M_Closure*
m_value_get_closure (M_Value v)
{
	assert(m_value_is_closure(v));

	return (M_Closure*) m_value_get_ptr(v);
}

/**
 * Get the array from the value.
 * \param v The value.
 * \return The array.
 */
static inline M_Array*
m_value_get_array (M_Value v)
{
	assert(m_value_is_array(v));

	return (M_Array*) m_value_get_ptr(v);
}

/**
 * Create a value from a boolean value.
 * \param b The boolean value.
 * \return The value.
 */
static inline M_Value
m_value_from_bool (M_Bool b)
{
	return b ? M_VALUE_TRUE : M_VALUE_FALSE;
}

/**
 * Create a value from an integer number.
 * \param i The integer number.
 * \return The value.
 */
static inline M_Value
m_value_from_int (int i)
{
#if __SIZEOF_POINTER__ == 4
	assert((i >= M_VALUE_INT_MIN) && (i <= M_VALUE_INT_MAX));
#endif
	return (((M_Value)i) << M_VALUE_TYPE_SHIFT) | M_VALUE_TYPE_INT;
}

/**
 * Create a value from a double precision number.
 * \param d The double precision number.
 * \return The value.
 */
static inline M_Value
m_value_from_double (double d)
{
	double *pd;

	pd = m_gc_alloc_obj(M_GC_OBJ_DOUBLE);
	m_assert_alloc(pd);

	*pd = d;

	return (M_Value)pd;
}

/**
 * Create a value from a number.
 * \param d The number.
 * \return The value.
 */
static inline M_Value
m_value_from_number (double d)
{
	int i = (int)d;

	if ((double)i == d) {
#if __SIZEOF_POINTER__ == 4
		if ((i >= M_VALUE_INT_MIN) && (i <= M_VALUE_INT_MAX));
#endif
		return m_value_from_int(i);
	}

	return m_value_from_double(d);
}

/**
 * Create a value from a string.
 * \param str The string.
 * \return The value.
 */
static inline M_Value
m_value_from_string (M_String *str)
{
	return ((M_Value)str) | M_VALUE_TYPE_STRING;
}

/**
 * Create a value from a pointer.
 * \param ptr The pointer.
 * \return The value.
 */
static inline M_Value
m_value_from_ptr (void *ptr)
{
	return (M_Value)ptr;
}

/**
 * Create a value from an object.
 * \param obj The object.
 * \return The value.
 */
static inline M_Value
m_value_from_object (M_Object *obj)
{
	uintptr_t *ptr;

	ptr = m_gc_alloc_obj(M_GC_OBJ_PTR);
	m_assert_alloc(ptr);

	*ptr = ((uintptr_t)obj) | M_PTR_TYPE_OBJECT;

	return m_value_from_ptr((void*)ptr);
}

/**
 * Create a value from a closure.
 * \param clos The closure.
 * \return The value.
 */
static inline M_Value
m_value_from_closure (M_Closure *clos)
{
	uintptr_t *ptr;

	ptr = m_gc_alloc_obj(M_GC_OBJ_PTR);
	m_assert_alloc(ptr);

	*ptr = ((uintptr_t)clos) | M_PTR_TYPE_CLOSURE;

	return m_value_from_ptr((void*)ptr);
}

/**
 * Create a value from an array.
 * \param array The array.
 * \return The value.
 */
static inline M_Value
m_value_from_array (M_Array *array)
{
	uintptr_t *ptr;

	ptr = m_gc_alloc_obj(M_GC_OBJ_PTR);
	m_assert_alloc(ptr);

	*ptr = ((uintptr_t)array) | M_PTR_TYPE_ARRAY;

	return m_value_from_ptr((void*)ptr);
}

#ifdef __cplusplus
}
#endif

#endif
