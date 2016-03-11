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
 * Hash table.
 */

#ifndef _M_HASH_H_
#define _M_HASH_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <m_types.h>

/**Hash table node.*/
typedef struct M_HashNode_s M_HashNode;
/**Hash table node.*/
struct M_HashNode_s {
	M_HashNode *next;   /**< The next node in the list.*/
};

/**Hash table.*/
typedef struct {
	M_HashNode **lists; /**< Node lists buffer.*/
	uint32_t     size;  /**< Nodes count in the hash table.*/
	uint32_t     nlist; /**< Number of lists.*/
} M_Hash;

/**Hash table operation functions.*/
typedef struct {
	/**Get key from the node.*/
	void*    (*get_key)(const M_HashNode *node);
	/**Key value calculate function.*/
	uint32_t (*kv)(const void *key);
	/**Compare if 2 keys are equal.*/
	M_Bool   (*equal)(const void *key1, const void *key2);
	/**Free node function invoked in "m_hash_deinit".*/
	void     (*free_node)(void *node);
	/**Allocate list buffer function invoked in "m_hash_resize".*/
	void*    (*alloc_buf)(size_t size);
	/**Free list buffer function invoked in "m_hash_deinit"*/
	void     (*free_buf)(void *ptr, size_t size);
} M_HashOps;

/**
 * Traverse the nodes in the hash table.
 * \a node is M_HashNode pointer pointed to each node.
 * \a pos is an integer to store the node list index.
 * \a hash is the hash table's pointer.
 * \code{.c}
 * M_Hash hash;
 * M_HashNode *node;
 * uint32_t pos;
 *
 * m_hash_init(&hash);
 * insert_nodes(&hash);
 *
 * m_hash_foreach(node, pos, &hash) {
 *		do_something(node);
 * }
 * \endcode
 */
#define m_hash_foreach(node, pos, hash)\
	for (pos = 0; pos < (hash)->nlist; (pos) ++)\
		for (node = (hash)->lists[pos]; node; node = (node)->next)

/**
 * Traverse the nodes in the hash table safely.
 * This macro store the next node pointer before the traverse block.
 * You can remove current node safely.
 * \a node is M_HashNode pointer pointed to each node.
 * \a nnext is M_HashNode pointer store the next next.
 * \a pos is an integer to store the node list index.
 * \a hash is the hash table's pointer.
 * \code{.c}
 * M_Hash hash;
 * M_HashNode *node, *nnode;
 * uint32_t pos;
 *
 * m_hash_init(&hash);
 * insert_nodes(&hash);
 *
 * m_hash_foreach_safe(node, nnode, pos, &hash) {
 *		do_something(node);
 *		m_hash_remove(&hash, get_key_from_node(node), &my_hash_ops);
 * }
 * \endcode
 */
#define m_hash_foreach_safe(node, nnext, pos, hash)\
	for (pos = 0; pos < (hash)->nlist; (pos) ++)\
		for (node = (hash)->lists[pos];\
					nnext = node ? (node)->next : NULL, node;\
					node = nnext)

/**
 * Traverse the nodes in the hash table with a parameter to store the
 * previous node's next filed's pointer.
 * You can use the next field's pointer to remove current node fastly.
 * \a node is M_HashNode pointer pointed to each node.
 * \a pnode is the previous node's next field pointer.
 * \a pos is an integer to store the node list index.
 * \a hash is the hash table's pointer.
 * \code{.c}
 * M_Hash hash;
 * M_HashNode *node, **prev;
 * uint32_t pos;
 *
 * m_hash_init(&hash);
 * insert_nodes(&hash);
 *
 * m_hash_foreach_prev(node, prev, pos, &hash) {
 *		do_something(node);
 *		m_hash_remove_from_prev(&hash, prev);
 * }
 * \endcode
 */
#define m_hash_foreach_prev(node, pnode, pos, hash)\
	for (pos = 0; pos < (hash)->nlist; (pos) ++)\
		for (pnode = &(hash)->lists[pos];\
					node = *(pnode);\
					pnode = (*(pnode) == node) ? &(node)->next : pnode)

/**
 * Traverse each value in the hash table.
 * \a val is value structure pointer pointed to each value.
 * \a pos is an integer to store the node list index.
 * \a hash is the hash table's pointer.
 * \a member is the member name of the hash node in the value structure.
 * \code{.c}
 * struct {
 *		M_HashNode hash_node;
 *		int        my_key;
 *		void      *my_data;
 * } MyHashValue;
 *
 * M_Hash hash;
 * MyHashValue *value;
 * uint32_t pos;
 *
 * m_hash_init(&hash);
 * insert_nodes(&hash);
 *
 * m_hash_foreach_value(value, pos, &hash, hash_node) {
 *		printf("key: %d\n", value->my_key);
 * }
 * \endcode
 */
#define m_hash_foreach_value(val, pos, hash, member)\
	for (pos = 0; pos < (hash)->nlist; (pos) ++)\
		for (val = m_node_value((hash)->lists[pos], typeof(*(val)), member);\
					val;\
					val = m_node_value((val)->member.next,\
						typeof(*(val)), member))

/**
 * Traverse each value in the hash table safely.
 * This macro store the next value pointer before the traverse block.
 * You can remove current value safely.
 * \a val is value structure pointer pointed to each value.
 * \a nval is the next value structure's pointer.
 * \a pos is an integer to store the node list index.
 * \a hash is the hash table's pointer.
 * \a member is the member name of the hash node in the value structure.
 * \code{.c}
 * struct {
 *		M_HashNode hash_node;
 *		int        my_key;
 *		void      *my_data;
 * } MyHashValue;
 *
 * M_Hash hash;
 * MyHashValue *value, *vnext;
 * uint32_t pos;
 *
 * m_hash_init(&hash);
 * insert_nodes(&hash);
 *
 * m_hash_foreach_value_safe(value, vnext, pos, &hash, hash_node) {
 *		printf("key: %d\n", value->my_key);
 *		if (value->my_key == 2) {
 *			m_hash_remove(&hash, M_SIZE_TO_PTR(2), &my_hash_ops);
 *		}
 * }
 * \endcode
 */
#define m_hash_foreach_value_safe(val, nval, pos, hash, member)\
	for (pos = 0; pos < (hash)->nlist; (pos) ++)\
		for (val = m_node_value((hash)->lists[pos], typeof(*(val)), member);\
					nval = (val) ? M_CONTAINER_OF((val)->member.next,\
						typeof(*(val)), member) : NULL,\
					val;\
					val = nval)

/**
 * Traverse the values in the hash table with a parameter to store the
 * previous node's next filed's pointer.
 * You can use the next field's pointer to remove current node fastly.
 * \a val is value structure pointer pointed to each value.
 * \a pnode is the previous node's next field pointer.
 * \a pos is an integer to store the node list index.
 * \a hash is the hash table's pointer.
 * \a member is the member name of the hash node in the value structure.
 * \code{.c}
 * struct {
 *		M_HashNode hash_node;
 *		int        my_key;
 *		void      *my_data;
 * } MyHashValue;
 *
 * M_Hash hash;
 * MyHashValue *value;
 * M_HashNode *prev;
 * uint32_t pos;
 *
 * m_hash_init(&hash);
 * insert_nodes(&hash);
 *
 * m_hash_foreach_value_prev(value, prev, pos, &hash, hash_node) {
 *		printf("key: %d\n", value->my_key);
 *		if (value->my_key == 2) {
 *			m_hash_remove_from_prev(&hash, prev);
 *		}
 * }
 *\endcode
 */
#define m_hash_foreach_value_prev(val, pnode, pos, hash, member)\
	for (pos = 0; pos < (hash)->nlist; (pos) ++)\
		for (pnode = &(hash)->lists[pos];\
					val = *(pnode) ? M_CONTAINER_OF(*(pnode),\
						typeof(*(val)), member) : NULL;\
					pnode = (*(pnode) == &(val)->member) ? &(val)->member.next : pnode)

/**
 * Get the nodes count in the hash table.
 * \param[in] hash The hash table.
 * \return The nodes count.
 */
static inline uint32_t
m_hash_size (M_Hash *hash)
{
	assert(hash);

	return hash->size;
}

/**
 * Hash table structure initialize.
 * \param[in] hash The hash table.
 */
static inline void
m_hash_init (M_Hash *hash)
{
	assert(hash);

	hash->lists = NULL;
	hash->size  = 0;
	hash->nlist = 0;
}

/**
 * Clear the hash table structure.
 * \param[in] hash The hash table.
 * \param[in] ops The hash table operation functions.
 */
static __always_inline void
m_hash_deinit (M_Hash *hash, const M_HashOps *ops)
{
	assert(hash && ops && ops->free_buf);

	if (hash->lists) {
		M_HashNode *node, *next;
		uint32_t pos;

		if (ops->free_node) {
			m_hash_foreach_safe(node, next, pos, hash) {
				ops->free_node(node);
			}
		}

		ops->free_buf(hash->lists, sizeof(M_HashNode*) * hash->nlist);
	}
}

/**
 * Lookup a node in the hash table.
 * \param[in] hash The hash table.
 * \param[in] key The key of the node.
 * \param[in] ops The hash table operation functions.
 * \param[out] pkv If \a pkv is not NULL, store the key value in it.
 * \return The node with the key find in the hahs table.
 * \retval NULL Cannot find the node with the key.
 */
static __always_inline M_HashNode*
m_hash_lookup_with_kv (M_Hash *hash, void *key, const M_HashOps *ops,
			uint32_t *pkv)
{
	M_HashNode *node;
	uint32_t pos;
	uint32_t kv;

	assert(hash && ops && ops->get_key && ops->kv && ops->equal);

	kv = ops->kv(key);
	if (pkv)
		*pkv = kv;

	if (hash->size) {
		pos = kv % hash->nlist;

		node = hash->lists[pos];
		while (node) {
			if (ops->equal(key, ops->get_key(node)))
				return node;

			node = node->next;
		}
	}

	return NULL;
}

/**
 * Lookup a node in the hash table.
 * \param[in] hash The hash table.
 * \param[in] key The key of the node.
 * \param[in] ops The hash table operation functions.
 * \param[out] pprev If \a pprev is not NULL,
 * store the previous node's next pointer.
 * \return The node with the key find in the hahs table.
 * \retval NULL Cannot find the node with the key.
 */
static __always_inline M_HashNode*
m_hash_lookup_with_prev (M_Hash *hash, void *key, const M_HashOps *ops,
			M_HashNode ***pprev)
{
	M_HashNode *node, **prev;
	uint32_t pos;
	uint32_t kv;

	assert(hash && ops && ops->get_key && ops->kv && ops->equal);

	kv = ops->kv(key);

	if (hash->size) {
		pos = kv % hash->nlist;

		prev = &hash->lists[pos];
		node = *prev;
		while (node) {
			if (ops->equal(key, ops->get_key(node))) {
				if (pprev)
					*pprev = prev;
				return node;
			}

			prev = &node->next;
			node = *prev;
		}
	}

	return NULL;
}

/**
 * Lookup a node in the hash table.
 * \param[in] hash The hash table.
 * \param[in] key The key of the node.
 * \param[in] ops The hash table operation functions.
 * \return The node with the key find in the hahs table.
 * \retval NULL Cannot find the node with the key.
 */
static __always_inline M_HashNode*
m_hash_lookup (M_Hash *hash, void *key, const M_HashOps *ops)
{
	return m_hash_lookup_with_kv(hash, key, ops, NULL);
}

/**
 * Remove a node from the hash table.
 * This function do not invoke ops->free_node to free the node.
 * \param[in] hash The hash table.
 * \param[in] key The key of the node.
 * \param[in] ops The hash table operation functions.
 * \return The node removed.
 * \retval NULL Cannot find the node with the key.
 */
static __always_inline M_HashNode*
m_hash_remove (M_Hash *hash, void *key, const M_HashOps *ops)
{
	M_HashNode *node, **pnode;
	uint32_t pos;

	assert(hash && ops && ops->get_key && ops->kv && ops->equal);

	if (hash->size) {
		pos = ops->kv(key) % hash->nlist;

		pnode = &hash->lists[pos];
		while (*pnode) {
			node = *pnode;

			if (ops->equal(key, ops->get_key(node))) {
				*pnode = node->next;
				hash->size --;
				return node;
			}

			pnode = &node->next;
		}
	}

	return NULL;
}

/**
 * Remove a node by its' previous node's next field pointer.
 * \param[in] hash The hash table.
 * \param[in] pnode The previous node's next field pointer.
 * \return The node removed.
 */
static inline M_HashNode*
m_hash_remove_from_prev (M_Hash *hash, M_HashNode **pnode)
{
	M_HashNode *node;

	assert(hash && pnode);

	node = *pnode;

	assert(node);

	*pnode = node->next;

	hash->size --;

	return node;
}

/**
 * Insert a node to the hash table.
 * When you want to add a new node into a hash table. You should lookup its
 * key in the hash table to comfirm the key is unique. And "m_hash_size"
 * should be invoked to check if the hash table need to be resized. The sample
 * code is as follows.
 * \code{.c}
 * uint32_t kv;
 * M_HashNode *node;
 *
 * node = m_hash_lookup_with_kv(hash, M_SIZE_TO_PTR(100), &my_hash_ops, &kv);
 * if (!node) {
 *		m_hash_resize(hash);
 *		m_hash_insert_with_kv(hash, M_SIZE_TO_PTR(100), kv, &my_hash_ops);
 * }
 *
 * \endcode
 * \param[in] hash The hash table.
 * \param[in] node The node to be added.
 * \param kv The key value of the node.
 * \param[in] ops The hash table operation functions.
 */
static __always_inline void
m_hash_insert_with_kv (M_Hash *hash, M_HashNode *node, uint32_t kv,
			const M_HashOps *ops)
{
	uint32_t pos;

	assert(hash && node && ops && ops->get_key && ops->kv);

	assert(hash->nlist);

	pos = kv % hash->nlist;

	node->next = hash->lists[pos];
	hash->lists[pos] = node;

	hash->size ++;
}

/**
 * Insert a node to the hash table.
 * When you want to add a new node into a hash table. You should lookup its
 * key in the hash table to comfirm the key is unique. And "m_hash_size"
 * should be invoked to check if the hash table need to be resized. The sample
 * code is as follows.
 * \code{.c}
 * M_HashNode *node;
 *
 * node = m_hash_lookup(hash, M_SIZE_TO_PTR(100), &my_hash_ops);
 * if (!node) {
 *		m_hash_resize(hash);
 *		m_hash_insert(hash, M_SIZE_TO_PTR(100), &my_hash_ops);
 * }
 *
 * \endcode
 * \param[in] hash The hash table.
 * \param[in] node The node to be added.
 * \param[in] ops The hash table operation functions.
 */
static __always_inline void
m_hash_insert (M_Hash *hash, M_HashNode *node, const M_HashOps *ops)
{
	uint32_t kv;

	assert(ops && ops->get_key && ops->kv);

	kv = ops->kv(ops->get_key(node));

	m_hash_insert_with_kv(hash, node, kv, ops);
}

/**
 * Check and extend the list buffer size in the hash table.
 * \param[in] hash The hash table.
 * \param[in] ops The hash table operation functions.
 * \retval M_OK On success.
 * \retval M_ERR_NO_MEM Not enough memory to extend the list buffer.
 */
static __always_inline M_Result
m_hash_resize (M_Hash *hash, const M_HashOps *ops)
{
	assert(hash && ops->alloc_buf && ops->free_buf && ops->get_key
				&& ops->kv);

	if (hash->nlist * 3 <= hash->size) {
		M_HashNode **nbuf;
		uint32_t nsize, pos;
		M_HashNode *node, *next;

		nsize = M_MAX(hash->size, 9);
		nbuf  = ops->alloc_buf(sizeof(M_HashNode*) * nsize);
		if (!nbuf)
			return M_ERR_NO_MEM;

		memset(nbuf, 0, sizeof(M_HashNode*) * nsize);

		m_hash_foreach_safe(node, next, pos, hash) {
			uint32_t i = ops->kv(ops->get_key(node)) % nsize;

			node->next = nbuf[i];
			nbuf[i]    = node;
		}

		if (hash->lists)
			ops->free_buf(hash->lists, sizeof(M_HashNode*) * hash->nlist);

		hash->lists = nbuf;
		hash->nlist = nsize;
	}

	return M_OK;
}

/**
 * Integer number key value calculate function.
 * \param[in] key Integer key.
 * \return Key value.
 */
static inline uint32_t
m_int_hash_kv_func (const void *key)
{
	return M_PTR_TO_SIZE(key);
}

/**
 * Integer number keys equal compare function.
 * \param[in] key1 Key 1.
 * \param[in] key2 Key 2.
 * \retval M_TRUE keys are equal.
 * \retval M_FALSE keys are not equal.
 */
static inline M_Bool
m_int_hash_equal_func (const void *key1, const void *key2)
{
	return M_PTR_TO_SIZE(key1) == M_PTR_TO_SIZE(key2);
}

/**
 * Pointer key value calculate function.
 * \param[in] key Pointer key.
 * \return Key value.
 */
static inline uint32_t
m_ptr_hash_kv_func (const void *key)
{
	return M_PTR_TO_SIZE(key);
}

/**
 * Pointer keys equal compare function.
 * \param[in] key1 Key 1.
 * \param[in] key2 Key 2.
 * \retval M_TRUE keys are equal.
 * \retval M_FALSE keys are not equal.
 */
static inline M_Bool
m_ptr_hash_equal_func (const void *key1, const void *key2)
{
	return key1 == key2;
}

#ifdef __cplusplus
}
#endif

#endif

