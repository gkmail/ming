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
 * List.
 */

#ifndef _M_LIST_H_
#define _M_LIST_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <m_types.h>

/**Double linked list node.*/
typedef struct M_List_s M_List;

/**Double linked list node.*/
struct M_List_s {
	M_List *prev; /**< The previous node in the list.*/
	M_List *next; /**< The next node in the list.*/
};

/**
 * Traverse the nodes in the list.
 */
#define m_list_foreach(node, head)\
	for (node = (head)->next; node != (head); node = (node)->next)

/**
 * Traverse the nodes in the list reversely.
 */
#define m_list_foreach_r(node, head)\
	for (node = (head)->prev; node != (head); node = (node)->prev)

/**
 * Traverse the nodes in the list safely.
 * The next node pointer is strored in \a nnext.
 * You can remove current node safely.
 */
#define m_list_foreach_safe(node, nnext, head)\
	for (node = (head)->next;\
				nnext = (node)->next, node != (head);\
				node = nnext)

/**
 * Traverse the nodes in the list reversely and safely.
 * The previous node pointer is strored in \a nprev.
 * You can remove current node safely.
 */
#define m_list_foreach_safe_r(node, nprev, head)\
	for (node = (head)->prev;\
				nprev = (node)->prev, node != (head);\
				node = nprev)

/**
 * Traverse the values in the list.
 */
#define m_list_foreach_value(val, head, member)\
	for (val = m_node_value((head)->next, typeof(*(val)), member);\
				val != m_node_value(head, typeof(*(val)), member);\
				val = m_node_value((val)->member.next, typeof(*(val)), member))

/**
 * Traverse the values in the list reversely.
 */
#define m_list_foreach_value_r(val, head, member)\
	for (val = m_node_value((head)->prev, typeof(*(val)), member);\
				val != m_node_value(head, typeof(*(val)), member);\
				val = m_node_value((val)->member.prev, typeof(*(val)), member))

/**
 * Traverse the values in the list safely.
 * The next node pointer is strored in \a nnext.
 * You can remove current node safely.
 */
#define m_list_foreach_value_safe(val, nval, head, member)\
	for (val = m_node_value((head)->next, typeof(*(val)), member);\
				nval = m_node_value((val)->member.next, typeof(*(val)), member),\
				val != m_node_value(head, typeof(*(val)), member);\
				val = nval)

/**
 * Traverse the values in the list reversely and safely.
 * The previous node pointer is strored in \a nprev.
 * You can remove current node safely.
 */
#define m_list_foreach_value_safe_r(val, pval, head, member)\
	for (val = m_node_value((head)->prev, typeof(*(val)), member);\
				pval = m_node_value((val)->member.prev, typeof(*(val)), member),\
				val != m_node_value(head, typeof(*(val)), member);\
				val = pval)

/**
 * Check if the list is empry.
 * \param[in] head The list head.
 * \retval M_TRUE The list is empty.
 * \retval M_FALSE The list is not empty.
 */
static inline M_Bool
m_list_empty (M_List *head)
{
	return head->next == head;
}

/**
 * List head initialize.
 * \param[in] head The list head.
 */
static inline void
m_list_init (M_List *head)
{
	assert(head);

	head->prev = head;
	head->next = head;
}

/**
 * Append a node into the list.
 * \param[in] head The list's head.
 * \param[in] node The new node to be added.
 */
static inline void
m_list_append (M_List *head, M_List *node)
{
	assert(head && node);

	node->prev = head->prev;
	node->next = head;
	head->prev->next = node;
	head->prev = node;
}

/**
 * Prepend a node into the list.
 * \param[in] head The list's head.
 * \param[in] node The new node to be added.
 */
static inline void
m_list_prepend (M_List *head, M_List *node)
{
	assert(head && node);

	node->prev = head;
	node->next = head->next;
	head->next->prev = node;
	head->next = node;
}

/**
 * Remove a node from the list.
 * \param[in] node The node to be removed.
 */
static inline void
m_list_remove (M_List *node)
{
	node->prev->next = node->next;
	node->next->prev = node->prev;
}

/**Single linked list node.*/
typedef struct M_SList_s M_SList;

/**Single linked list node.*/
struct M_SList_s {
	M_SList *next; /**< The next node in the list.*/
};

/**
 * Traverse the nodes in the single linked list.
 */
#define m_slist_foreach(node, head)\
	for (node = (head)->next; node; node = (node)->next)

/**
 * Traverse the nodes in the single linked list safely.
 * The next pointer is stored in \a nnext.
 * You can free the current node safely.
 */
#define m_slist_foreach_safe(node, nnext, head)\
	for (node = (head)->next;\
				nnext = node ? (node)->next : NULL, node;\
				node = nnext)

/**
 * Traverse the values in the single linked list.
 */
#define m_slist_foreach_value(val, head, member)\
	for (val = m_node_value((head)->next, typeof(*(val)), member);\
				val;\
				val = m_node_value((val)->member.next,\
					typeof(*(val)), member))

/**
 * Traverse the values in the single linked list safely.
 * The next value's pointer is stored in \a nval.
 * You can free the current value safely.
 */
#define m_slist_foreach_value_safe(val, nval, head, member)\
	for (val = m_node_value((head)->next, typeof(*(val)), member);\
				nval = (val) ? m_node_value((val)->member.next,\
					typeof(*(val)), member) : NULL, val;\
				val = nval)


/**
 * Single linked list head initialize.
 * \param[in] head The list head.
 */
static inline void
m_slist_init (M_SList *head)
{
	assert(head);

	head->next = NULL;
}

/**
 * Check if the single linked list is empry.
 * \param[in] head The list head.
 * \retval M_TRUE The list is empty.
 * \retval M_FALSE The list is not empty.
 */
static inline M_Bool
m_slist_empty (M_SList *head)
{
	return head->next == NULL;
}

/**
 * Push a new node into the single linked list.
 * \param[in] head The single linked list head.
 * \param[in] node The new node to be added.
 */
static inline void
m_slist_push (M_SList *head, M_SList *node)
{
	assert(head && node);

	node->next = head->next;
	head->next = node;
}

/**
 * Pop the head node from the single linked list.
 * \param[in] head The single list head.
 * \return The popped node.
 */
static inline M_SList*
m_slist_pop (M_SList *head)
{
	M_SList *node;

	assert(head);

	node = head->next;
	if (node)
		head->next = node->next;

	return node;
}

#ifdef __cplusplus
}
#endif

#endif
