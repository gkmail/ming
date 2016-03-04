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

#define M_LOG_TAG "ming"

#include <ming.h>

static void
hash_info (M_Hash *hash)
{
	M_HashNode *node;
	uint32_t pos;
	int max = 0;

	for (pos = 0; pos < hash->nlist; pos ++) {
		int len = 0;

		for (node = hash->lists[pos]; node; node = node->next) {
			len ++;
		}

		max = M_MAX(max, len);
	}

	printf("hash size: %d nlist: %d max list len: %d\n",
				hash->size, hash->nlist, max);
}

typedef struct {
	M_HashNode node;
	int        i;
} IntHashNode;

static inline void*
int_get_key(const M_HashNode *node)
{
	return M_SIZE_TO_PTR(M_CONTAINER_OF(node, IntHashNode, node)->i);
}

static const M_HashOps int_ops = {
get_key:   int_get_key,
kv:        m_int_hash_kv_func,
equal:     m_int_hash_equal_func,
free_node: m_free,
alloc_buf: m_malloc,
free_buf:  (void*)m_free
};

static void
int_hash_test (void)
{
#define INT_COUNT 1024*1024

	M_Hash hash;
	M_HashNode *node, *next, **prev;
	IntHashNode *in, *nin;
	uint32_t pos;
	int i, count = INT_COUNT;
	static M_Bool flags[INT_COUNT];

	M_INFO("int hash test begin");

	m_hash_init(&hash);

	for (i = 0; i < count; i ++) {
		uint32_t kv;

		node = m_hash_lookup_with_kv(&hash, M_SIZE_TO_PTR(i), &int_ops, &kv);
		if (node) {
			M_ERROR("lookup error");
		}

		m_hash_resize(&hash, &int_ops);

		in = M_NEW(IntHashNode, 1);
		in->i = i;

		m_hash_insert_with_kv(&hash, &in->node, kv, &int_ops);

		if (m_hash_size(&hash) != i + 1) {
			M_ERROR("hash size error");
		}
	}

	hash_info(&hash);

	for (i = 0; i < count; i ++) {
		node = m_hash_lookup(&hash, M_SIZE_TO_PTR(i), &int_ops);
		if (!node) {
			M_ERROR("cannot find inserted node");
		}

		if (M_CONTAINER_OF(node, IntHashNode, node)->i != i) {
			M_ERROR("hash key error");
		}
	}

	for (i = 0; i < count; i ++) {
		node = m_hash_remove(&hash, M_SIZE_TO_PTR(i), &int_ops);
		if (!node) {
			M_ERROR("cannot remove entry");
		}

		m_free(M_CONTAINER_OF(node, IntHashNode, node));

		if (m_hash_size(&hash) != count - i - 1) {
			M_ERROR("hash size error");
		}
	}

	for (i = 0; i < count; i ++) {
		in = M_NEW(IntHashNode, 1);
		in->i = i;

		m_hash_insert(&hash, &in->node, &int_ops);
	}

	memset(flags, 0, sizeof(flags));
	m_hash_foreach(node, pos, &hash) {
		i = M_CONTAINER_OF(node, IntHashNode, node)->i;
		if (flags[i]) {
			M_ERROR("traverse error");
		}

		flags[i] = M_TRUE;
	}

	for (i = 0; i < count; i ++) {
		if (!flags[i])
			M_ERROR("traverse error");
	}

	m_hash_foreach_safe(node, next, pos, &hash) {
		i = M_CONTAINER_OF(node, IntHashNode, node)->i;
		if (i & 1) {
			m_hash_remove(&hash, M_SIZE_TO_PTR(i), &int_ops);
			m_free(M_CONTAINER_OF(node, IntHashNode, node));
		}
	}

	for (i = 0; i < count; i++) {
		if (i & 1) {
			node = m_hash_lookup(&hash, M_SIZE_TO_PTR(i), &int_ops);
			if (node)
				M_ERROR("hash error after remove");
		} else {
			node = m_hash_lookup(&hash, M_SIZE_TO_PTR(i), &int_ops);
			if (!node)
				M_ERROR("hash error after remove");
		}
	}
	
	m_hash_foreach_prev(node, prev, pos, &hash) {
		i = M_CONTAINER_OF(node, IntHashNode, node)->i;

		if ((i % 4) == 0) {
			m_hash_remove_from_prev(&hash, prev);
			m_free(M_CONTAINER_OF(node, IntHashNode, node));
		}
	}

	for (i = 0; i < count; i++) {
		if ((i & 1) || !(i % 4)) {
			node = m_hash_lookup(&hash, M_SIZE_TO_PTR(i), &int_ops);
			if (node)
				M_ERROR("hash error after remove");
		} else {
			node = m_hash_lookup(&hash, M_SIZE_TO_PTR(i), &int_ops);
			if (!node)
				M_ERROR("hash error after remove");
		}
	}

	m_hash_foreach_value(in, pos, &hash, node) {
		if ((in->i & 1) || !(in->i % 4)) {
			M_ERROR("hash error after remove");
		}
	}

	m_hash_foreach_value_prev(in, prev, pos, &hash, node) {
		if ((in->i % 4) == 2) {
			m_hash_remove_from_prev(&hash, prev);
			m_free(in);
		}
	}

	m_hash_foreach_value(in, pos, &hash, node) {
		if ((in->i & 1) || !(in->i % 4) || ((in->i % 4) == 2)) {
			M_ERROR("hash error after remove");
		}
	}

	m_hash_foreach_value_safe(in, nin, pos, &hash, node) {
		m_hash_remove(&hash, M_SIZE_TO_PTR(in->i), &int_ops);
		m_free(in);
	}

	m_hash_deinit(&hash, &int_ops);

	M_INFO("int hash test end");
}

typedef struct {
	M_HashNode node;
	void      *ptr;
	int        v;
} PtrHashNode;

static inline void*
ptr_get_key(const M_HashNode *node)
{
	return M_CONTAINER_OF(node, PtrHashNode, node)->ptr;
}

static const M_HashOps ptr_ops = {
get_key:   ptr_get_key,
kv:        m_ptr_hash_kv_func,
equal:     m_ptr_hash_equal_func,
free_node: m_free,
alloc_buf: m_malloc,
free_buf:  (void*)m_free
};

static void
ptr_hash_test (void)
{
#define PTR_COUNT 1024*1024
	M_Hash hash;
	M_HashNode *node;
	PtrHashNode *pn, *pn_next;
	static void *ptrs[PTR_COUNT];
	int count = PTR_COUNT;
	int i;
	uint32_t kv, pos;

	M_INFO("ptr hash test begin");

	m_hash_init(&hash);

	for (i = 0; i < count; i ++) {
		ptrs[i] = m_malloc(16);
	}

	for (i = 0; i < count; i ++) {
		node = m_hash_lookup_with_kv(&hash, ptrs[i], &ptr_ops, &kv);
		if (node) {
			M_ERROR("lookup error");
		}

		m_hash_resize(&hash, &ptr_ops);
		
		pn = M_NEW(PtrHashNode, 1);
		pn->ptr = ptrs[i];
		pn->v   = i;

		m_hash_insert_with_kv(&hash, &pn->node, kv, &ptr_ops);
	}

	hash_info(&hash);

	for (i = 0; i < count; i ++) {
		node = m_hash_lookup(&hash, ptrs[i], &ptr_ops);
		if (!node) {
			M_ERROR("lookup error");
		}

		if (M_CONTAINER_OF(node, PtrHashNode, node)->v != i) {
			M_ERROR("node value error");
		}
	}

	m_hash_foreach_value_safe(pn, pn_next, pos, &hash, node) {
		if (pn->v & 1) {
			m_hash_remove(&hash, pn->ptr, &ptr_ops);
			m_free(pn);
		}
	}

	m_hash_foreach_value(pn, pos, &hash, node) {
		if (pn->v & 1) {
			M_ERROR("remove error");
		}
	}

	for (i = 0; i < count; i ++) {
		m_free(ptrs[i]);
	}

	m_hash_deinit(&hash, &ptr_ops);

	M_INFO("ptr hash test end");
}

int
main (int argc, char **argv)
{
	m_startup();

	int_hash_test();
	ptr_hash_test();

	return 0;
}

