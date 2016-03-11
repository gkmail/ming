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
 * Red black tree.
 */

#ifndef _M_RBTREE_H_
#define _M_RBTREE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <m_types.h>

/**RB tree node.*/
typedef struct M_RBNode_s M_RBNode;

/**RB tree node.*/
struct M_RBNode_s {
	/**Parent node and color flag. color store in the lowest bit.*/
	uintptr_t  pcolor;
	/**The left node.*/
	M_RBNode  *left;
	/**The right node.*/
	M_RBNode  *right;
};

/**Red black tree.*/
typedef M_RBNode* M_RBTree;

/**Red black tree operation functions.*/
typedef struct {
	/**Get key from node.*/
	void* (*get_key) (const M_RBNode *node);
	/**Key value compare function.*/
	int   (*cmp) (const void *key1, const void *key2);
	/**Node free function. Invoked in "m_rbt_deinit" function.*/
	void  (*free_node) (void *ptr);
} M_RBTreeOps;

/**
 * Traverse the nodes in the RB tree.
 * \code{.c}
 * M_RBTree tree;
 * M_RBNode *node;
 *
 * m_rbt_init(&tree);
 * add_nodes(&tree);
 *
 * m_rbt_foreach(node, &tree) {
 *		do_something(node);
 * }
 * \endcode
 * \a node is the node pointer pointed to the current node.
 * \a tree is the RB tree pointer.
 */
#define m_rbt_foreach(node, tree)\
	for (node = m_rbt_first_node(tree); node; node = m_rbt_next_node(node))

/**
 * Traverse the nodes in the RB tree safely.
 * The next node pointer is stroed, and you can invoke m_rbt_remove safely.
 * \code{.c}
 * M_RBTree tree;
 * M_RBNode *node, *nnode;
 *
 * m_rbt_init(&tree);
 * add_nodes(&tree);
 *
 * m_rbt_foreach_safe(node, nnode, &tree) {
 *		do_something(node);
 *		m_rbt_remove(&tree, node);
 * }
 * \endcode
 * \a node is the node pointer pointed to the current node.
 * \a nnext is the next node pointer.
 * \a tree is the RB tree pointer.
 */
#define m_rbt_foreach_safe(node, nnext, tree)\
	for (node = m_rbt_first_node(tree);\
				nnext = m_rbt_next_node(node), node;\
				node = nnext)

/**
 * Traverse the values in the RB tree.
 * \code{.c}
 * typedef struct {
 *		M_RBNode rb_node;
 *		int      key;
 * } MyNode;
 *
 * M_RBTree tree;
 * MyNode *node;
 *
 * m_rbt_init(&tree);
 * add_nodes(&tree);
 *
 * m_rbt_foreach_value(node, &tree. rb_node) {
 *		printf("key: %d\n", node->key);
 * }
 * \endcode
 * \a node is the node pointer pointed to the current node.
 * \a tree is the RB tree pointer.
 */
#define m_rbt_foreach_value(val, tree, member)\
	for (val = m_node_value(m_rbt_first_node(tree), typeof(*(val)), member);\
				val;\
				val = m_node_value(m_rbt_next_node(&(val)->member),\
					typeof(*(val)), member))

/**
 * Traverse the values in the RB tree safely.
 * The next node pointer is stroed, and you can invoke m_rbt_remove safely.
 * \code{.c}
 * typedef struct {
 *		M_RBNode rb_node;
 *		int      key;
 * } MyNode;
 *
 * M_RBTree tree;
 * MyNode *node, *nnode;
 *
 * m_rbt_init(&tree);
 * add_nodes(&tree);
 *
 * m_rbt_foreach_value_safe(node, nnode, &tree. rb_node) {
 *		printf("key: %d\n", node->key);
 *		m_rbt_remove(&tree, &node->rb_node);
 * }
 * \endcode
 * \a node is the node pointer pointed to the current node.
 * \a nnext is the next node pointer.
 * \a tree is the RB tree pointer.
 */
#define m_rbt_foreach_value_safe(val, nval, tree, member)\
	for (val = m_node_value(m_rbt_first_node(tree), typeof(*(val)), member);\
				nval = val ? m_node_value(m_rbt_next_node(&(val)->member),\
					typeof(*(val)), member)\
				: NULL, val;\
				val = nval)

/**
 * Traverse the nodes in the RB tree reversely.
 * \code{.c}
 * M_RBTree tree;
 * M_RBNode *node;
 *
 * m_rbt_init(&tree);
 * add_nodes(&tree);
 *
 * m_rbt_foreach_r(node, &tree) {
 *		do_something(node);
 * }
 * \endcode
 * \a node is the node pointer pointed to the current node.
 * \a tree is the RB tree pointer.
 */
#define m_rbt_foreach_r(node, tree)\
	for (node = m_rbt_last_node(tree); node; node = m_rbt_prev_node(node))

/**
 * Traverse the nodes in the RB tree safely and reversely.
 * The next node pointer is stroed, and you can invoke m_rbt_remove safely.
 * \code{.c}
 * M_RBTree tree;
 * M_RBNode *node, *nnode;
 *
 * m_rbt_init(&tree);
 * add_nodes(&tree);
 *
 * m_rbt_foreach_safe_r(node, nnode, &tree) {
 *		do_something(node);
 *		m_rbt_remove(&tree, node);
 * }
 * \endcode
 * \a node is the node pointer pointed to the current node.
 * \a nnext is the next node pointer.
 * \a tree is the RB tree pointer.
 */
#define m_rbt_foreach_safe_r(node, nnext, tree)\
	for (node = m_rbt_last_node(tree);\
				nnext = m_rbt_prev_node(node), node;\
				node = nnext)

/**
 * Traverse the values in the RB tree reversely.
 * \code{.c}
 * typedef struct {
 *		M_RBNode rb_node;
 *		int      key;
 * } MyNode;
 *
 * M_RBTree tree;
 * MyNode *node;
 *
 * m_rbt_init(&tree);
 * add_nodes(&tree);
 *
 * m_rbt_foreach_value_r(node, &tree. rb_node) {
 *		printf("key: %d\n", node->key);
 * }
 * \endcode
 * \a node is the node pointer pointed to the current node.
 * \a tree is the RB tree pointer.
 */
#define m_rbt_foreach_value_r(val, tree, member)\
	for (val = m_node_value(m_rbt_last_node(tree), typeof(*(val)), member);\
				val;\
				val = m_node_value(m_rbt_prev_node(&(val)->member),\
					typeof(*(val)), member))

/**
 * Traverse the values in the RB tree safely and reversely.
 * The next node pointer is stroed, and you can invoke m_rbt_remove safely.
 * \code{.c}
 * typedef struct {
 *		M_RBNode rb_node;
 *		int      key;
 * } MyNode;
 *
 * M_RBTree tree;
 * MyNode *node, *nnode;
 *
 * m_rbt_init(&tree);
 * add_nodes(&tree);
 *
 * m_rbt_foreach_value_safe_r(node, nnode, &tree. rb_node) {
 *		printf("key: %d\n", node->key);
 *		m_rbt_remove(&tree, &node->rb_node);
 * }
 * \endcode
 * \a node is the node pointer pointed to the current node.
 * \a nnext is the next node pointer.
 * \a tree is the RB tree pointer.
 */
#define m_rbt_foreach_value_safe_r(val, nval, tree, member)\
	for (val = m_node_value(m_rbt_last_node(tree), typeof(*(val)), member);\
				nval = val ? m_node_value(m_rbt_prev_node(&(val)->member),\
					typeof(*(val)), member)\
				: NULL, val;\
				val = nval)

/** \cond */
#define M_RB_FL_BLACK 1
#define M_RB_BLACK 1
#define M_RB_RED   0

static inline M_RBNode*
m_rbt_parent (M_RBNode *node)
{
	return (M_RBNode*)M_SIZE_TO_PTR(node->pcolor & ~M_RB_FL_BLACK);
}

static inline M_RBNode*
m_rbt_sibling (M_RBNode *node)
{
	M_RBNode *parent;

	parent = m_rbt_parent(node);

	return (parent->left == node) ? parent->right : parent->left;
}

static inline int
m_rbt_color (M_RBNode *node)
{
	return node ? node->pcolor & M_RB_FL_BLACK : M_RB_BLACK;
}

static inline void
m_rbt_set_color (M_RBNode *node, int color)
{
	if (color)
		node->pcolor |= M_RB_FL_BLACK;
	else
		node->pcolor &= ~M_RB_FL_BLACK;
}

static inline void
m_rbt_set_parent (M_RBNode *node, M_RBNode *parent)
{
	node->pcolor &= M_RB_FL_BLACK;
	node->pcolor |= M_PTR_TO_SIZE(parent);
}

static inline void
m_rbt_set_parent_color (M_RBNode *node, M_RBNode *parent, int color)
{
	node->pcolor = M_PTR_TO_SIZE(parent) | color;
}

static inline void
m_rbt_free_node (M_RBNode *node, void(*func)(void *node))
{
	if (node->left)
		m_rbt_free_node(node->left, func);
	if (node->right)
		m_rbt_free_node(node->right, func);

	func(node);
}

/** \endcond */

/**
 * Get the first node in the RB tree.
 * \param[in] tree The RB tree.
 * \return The first node in the tree.
 * \retval NULL The tree has not any node.
 */
static inline M_RBNode*
m_rbt_first_node (M_RBTree *tree)
{
	M_RBNode *node;

	assert(tree);

	node = *tree;
	if (!node)
		return NULL;

	while (node->left)
		node = node->left;

	return node;
}

/**
 * Get the last node in the RB tree.
 * \param[in] tree The RB tree.
 * \return The last node in the tree.
 * \retval NULL The tree has not any node.
 */
static inline M_RBNode*
m_rbt_last_node (M_RBTree *tree)
{
	M_RBNode *node;

	assert(tree);

	node = *tree;
	if (!node)
		return NULL;

	while (node->right)
		node = node->right;

	return node;
}

/**
 * Get the next node.
 * \param[in] node The current node.
 * \return The next node.
 * \retval NULL \a node is the last node.
 */
static inline M_RBNode*
m_rbt_next_node (M_RBNode *node)
{
	M_RBNode *next;

	if (!node)
		return NULL;

	next = node->right;
	if (next) {
		while (next->left)
			next = next->left;

		return next;
	}

	next = m_rbt_parent(node);
	while (next) {
		if (next->left == node)
			return next;

		node = next;
		next = m_rbt_parent(node);
	}

	return NULL;
}

/**
 * Get the previous node.
 * \param[in] node The current node.
 * \return The previous node.
 * \retval NULL \a node is the first node.
 */
static inline M_RBNode*
m_rbt_prev_node (M_RBNode *node)
{
	M_RBNode *prev;

	if (!node)
		return NULL;

	prev = node->left;
	if (prev) {
		while (prev->right)
			prev = prev->right;

		return prev;
	}

	prev = m_rbt_parent(node);
	while (prev) {
		if (prev->right == node)
			return prev;

		node = prev;
		prev = m_rbt_parent(node);
	}

	return NULL;
}

/**
 * RB tree structure innitialize.
 * \param[in] tree The RB tree.
 */
static inline void
m_rbt_init (M_RBTree *tree)
{
	assert(tree);

	*tree = NULL;
}

/**
 * Clear the RB tree structure.
 * \param[in] tree The RB tree.
 * \param[in] ops RB tree operation functions.
 */
static __always_inline void
m_rbt_deinit (M_RBTree *tree, const M_RBTreeOps *ops)
{
	assert(tree && ops);

	if (ops->free_node && *tree) {
		m_rbt_free_node(*tree, ops->free_node);
	}
}

/**
 * Lookup a node in the RB tree.
 * \param[in] tree The RB tree.
 * \param[in] key The key value.
 * \param[in] ops The RB tree operation functions.
 * \return The node find in the tree.
 * \retval NULL Cannot find the node with the key.
 */
static __always_inline M_RBNode*
m_rbt_lookup (M_RBTree *tree, void *key, const M_RBTreeOps *ops)
{
	M_RBNode *node;
	int v;

	assert(tree && ops && ops->get_key && ops->cmp);

	node = *tree;

	while (node) {
		v = ops->cmp(key, ops->get_key(node));

		if (v == 0)
			return node;
		else if (v < 0)
			node = node->left;
		else
			node = node->right;
	}

	return NULL;
}

/**
 * Lookup a node in the RB tree.
 * This is lookup function for insert operation.
 * Normal insert operation is as follows.
 * \code{.c}
 * M_RBNode *node, *parent, **pos, *new;
 *
 * node = m_rbt_lookup_insert(tree, M_SIZE_TO_PTR(100), &my_ops, &parent, &pos);
 * if (!node) {
 *		new = create_new_node();
 *		m_rbt_insert(tree, parent, pos, &new);
 * }
 * \endcode
 * \param[in] tree The RB tree.
 * \param[in] key The key value.
 * \param[in] ops The RB tree operation functions.
 * \param[out] parent If cannot find the node, return the parent node pointer
 * where to insert a new node.
 * \param[out] pos If cannot find the node, return the 
 * \return The node find in the tree.
 * \retval NULL Cannot find the node with the key.
 */
static __always_inline M_RBNode*
m_rbt_lookup_insert (M_RBTree *tree, void *key, const M_RBTreeOps *ops,
			M_RBNode **parent, M_RBNode ***pos)
{
	M_RBNode **pn, *node = NULL;
	int v;

	assert(tree && ops && ops->get_key && ops->cmp && parent && pos);

	pn = tree;

	while (*pn) {
		node = *pn;
		v = ops->cmp(key, ops->get_key(node));

		if (v == 0) {
			return node;
		} else if (v < 0) {
			pn = &node->left;
		} else {
			pn = &node->right;
		}
	}

	*parent = node;
	*pos = pn;

	return NULL;
}

/** \cond */
static inline void
m_rbt_rotate_left (M_RBTree *tree, M_RBNode *node)
{
	M_RBNode *x, *y, *child, *parent;

	x = node;
	y = x->right;

	assert(y);

	child = y->left;
	parent = m_rbt_parent(x);

	x->right = child;
	y->left  = x;
	if (parent) {
		if (parent->left == x)
			parent->left = y;
		else
			parent->right = y;
	} else {
		*tree = y;
	}

	if (child)
		m_rbt_set_parent(child, x);

	m_rbt_set_parent(x, y);
	m_rbt_set_parent(y, parent);
}

static inline void
m_rbt_rotate_right (M_RBTree *tree, M_RBNode *node)
{
	M_RBNode *x, *y, *child, *parent;

	x = node;
	y = x->left;

	assert(y);

	child = y->right;
	parent = m_rbt_parent(x);

	x->left = child;
	y->right  = x;
	if (parent) {
		if (parent->left == x)
			parent->left = y;
		else
			parent->right = y;
	} else {
		*tree = y;
	}

	if (child)
		m_rbt_set_parent(child, x);

	m_rbt_set_parent(x, y);
	m_rbt_set_parent(y, parent);
}

static inline void
m_rbt_insert (M_RBTree *tree, M_RBNode *parent, M_RBNode **pos,
			M_RBNode *node)
{
	assert(tree && pos && node);

	node->left  = NULL;
	node->right = NULL;

	*pos = node;
	m_rbt_set_parent_color(node, parent, M_RB_RED);

	/*Rebalance.*/
	while (1) {
		M_RBNode *parent, *gparent, *uncle;

		parent = m_rbt_parent(node);

		/*Root node, set to red and exit.*/
		if (parent == NULL) {
			m_rbt_set_color(node, M_RB_BLACK);
			return;
		}

		/*Parent is black, exit.*/
		if (m_rbt_color(parent) == M_RB_BLACK) {
			return;
		}

		gparent = m_rbt_parent(parent);

		assert(gparent);

		uncle = m_rbt_sibling(parent);

		/*Uncle is red.*/
		if (m_rbt_color(uncle) == M_RB_RED) {
			m_rbt_set_color(parent, M_RB_BLACK);
			m_rbt_set_color(uncle, M_RB_BLACK);
			m_rbt_set_color(gparent, M_RB_RED);
			node = gparent;
			continue;
		}

		if ((uncle == gparent->right) && (node == parent->right)) {
			m_rbt_rotate_left(tree, parent);
			M_EXCHANGE(node, parent);
		} else if ((uncle == gparent->left) && (node == parent->left)) {
			m_rbt_rotate_right(tree, parent);
			M_EXCHANGE(node, parent);
		}

		m_rbt_set_color(parent, M_RB_BLACK);
		m_rbt_set_color(gparent, M_RB_RED);

		if (uncle == gparent->right) {
			m_rbt_rotate_right(tree, gparent);
		} else {
			m_rbt_rotate_left(tree, gparent);
		}

		break;
	}
}

static __always_inline void
m_rbt_remove_rebalance (M_RBTree *tree, M_RBNode *node)
{
	M_RBNode *parent, *gparent, *sibling, *sleft, *sright;
	int scol, slcol, srcol, pcol;

	while (1) {
		/*Current is red, set it to black and exit.*/
		if (m_rbt_color(node) == M_RB_RED) {
			m_rbt_set_color(node, M_RB_BLACK);
			break;
		}

		/*Root node, exit.*/
		parent = m_rbt_parent(node);
		if (!parent)
			break;

		/*Sibling is red.*/
		sibling = m_rbt_sibling(node);
		scol = m_rbt_color(sibling);

		if (scol == M_RB_RED) {
			m_rbt_set_color(parent, M_RB_RED);
			m_rbt_set_color(sibling, M_RB_BLACK);

			if (node == parent->left)
				m_rbt_rotate_left(tree, parent);
			else
				m_rbt_rotate_right(tree, parent);

			continue;
		}

		sleft  = sibling ? sibling->left : NULL;
		sright = sibling ? sibling->right : NULL;
		slcol = m_rbt_color(sleft);
		srcol = m_rbt_color(sright);

		if ((slcol == M_RB_BLACK) && (srcol == M_RB_BLACK)) {
			if (sibling)
				m_rbt_set_color(sibling, M_RB_RED);
			node = parent;
			continue;
		}

		if ((node == parent->left) && (slcol == M_RB_RED)) {
			m_rbt_set_color(sleft, M_RB_BLACK);
			m_rbt_set_color(sibling, M_RB_RED);
			m_rbt_rotate_right(tree, sibling);
			sright  = sibling;
			sibling = sleft;
		} else if ((node == parent->right) && (srcol == M_RB_RED)) {
			m_rbt_set_color(sright, M_RB_BLACK);
			m_rbt_set_color(sibling, M_RB_RED);
			m_rbt_rotate_left(tree, sibling);
			sleft   = sibling;
			sibling = sright;
		}

		pcol = m_rbt_color(parent);
		m_rbt_set_color(parent, M_RB_BLACK);
		m_rbt_set_color(sibling, pcol);

		if (node == parent->left) {
			m_rbt_set_color(sright, M_RB_BLACK);
			m_rbt_rotate_left(tree, parent);
		} else {
			m_rbt_set_color(sleft, M_RB_BLACK);
			m_rbt_rotate_right(tree, parent);
		}

		break;
	}
}

/** \endcond */

/**
 * Remove a node from RB tree.
 * \param[in] tree The RB tree.
 * \param[in] node The node to be removed.
 */
static inline void
m_rbt_remove (M_RBTree *tree, M_RBNode *node)
{
	M_RBNode *succ, *child, *parent;
	M_Bool is_left;

	assert(tree && node);

	if (node->left && node->right) {
		/*Find the successor.*/
		succ = node->left;
		while (succ->right)
			succ = succ->right;

		if (succ == node->left) {
			parent  = succ;
			is_left = M_TRUE;
		} else {
			parent  = m_rbt_parent(succ);
			is_left = (parent->left == succ);
		}

		child  = succ->left ? succ->left : succ->right;
	} else {
		succ = node;

		parent = m_rbt_parent(node);
		child  = node->left ? node->left : node->right;
		if (parent)
			is_left = (parent->left == node);
	}

	if (!child) {
		/*Rebalance.*/
		m_rbt_remove_rebalance(tree, succ);
	}

	if (succ != node) {
		/*Swap successor and the real node.*/
		M_RBNode *left, *right, *oparent;
		uintptr_t pcolor;

		if (succ == node->left) {
			left = NULL;
		} else {
			left = node->left;
		}

		right  = node->right;
		pcolor = node->pcolor;
		oparent = m_rbt_parent(node);

		succ->left   = left;
		succ->right  = right;
		succ->pcolor = pcolor;

		if (left)
			m_rbt_set_parent(left, succ);
		if (right)
			m_rbt_set_parent(right, succ);
		if (oparent) {
			if (oparent->left == node)
				oparent->left = succ;
			else
				oparent->right = succ;
		} else {
			*tree = succ;
		}
	}

	/*Remove the node*/
	if (parent) {
		if (is_left)
			parent->left = child;
		else
			parent->right = child;
	} else {
		*tree = child;
	}

	if (child) {
		m_rbt_set_parent_color(child, parent, M_RB_BLACK);
	}
}

#ifdef __cplusplus
}
#endif

#endif
