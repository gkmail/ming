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
 * Compare functions.
 */

#ifndef _M_CMP_H_
#define _M_CMP_H_

#ifdef __cplusplus
extern "C" {
#endif

/**Compare function.*/
typedef int (*M_CmpFunc) (const void *v1, const void *v2);

/**
 * Integer number compare function.
 * \param[in] key1 Integer number 1.
 * \param[in] key2 Integer number 2.
 * \retval 0 key1 == key2.
 * \retval <0 key1 < key2.
 * \retval >0 key1 > key2.
 */
static inline int
m_int_cmp_func (const void *key1, const void *key2)
{
	return M_PTR_TO_SIZE(key1) - M_PTR_TO_SIZE(key2);
}

/**
 * Pointer compare function.
 * \param[in] key1 Pointer 1.
 * \param[in] key2 Pointer 2.
 * \retval 0 key1 == key2.
 * \retval <0 key1 < key2.
 * \retval >0 key1 > key2.
 */
static inline int
m_ptr_cmp_func (const void *key1, const void *key2)
{
	return M_PTR_TO_SIZE(key1) - M_PTR_TO_SIZE(key2);
}

/**
 * C string compare function.
 * \param[in] key1 String 1.
 * \param[in] key2 String 2.
 * \retval 0 key1 == key2.
 * \retval <0 key1 < key2.
 * \retval >0 key1 > key2.
 */
static inline int
m_cstr_cmp_func (const void *key1, const void *key2)
{
	char *s1 = (char*)key1;
	char *s2 = (char*)key2;

	if (s1 == s2)
		return 0;

	if (!s1)
		return -1;
	if (!s2)
		return 1;

	return strcmp(s1, s2);
}

/**
 * C string compare function ignoring case.
 * \param[in] key1 String 1.
 * \param[in] key2 String 2.
 * \retval 0 key1 == key2.
 * \retval <0 key1 < key2.
 * \retval >0 key1 > key2.
 */
static inline int
m_cstr_casecmp_func (const void *key1, const void *key2)
{
	char *s1 = (char*)key1;
	char *s2 = (char*)key2;

	if (s1 == s2)
		return 0;

	if (!s1)
		return -1;
	if (!s2)
		return 1;

	return strcasecmp(s1, s2);
}

#ifdef __cplusplus
}
#endif

#endif
