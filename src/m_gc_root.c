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

#define M_LOG_TAG "gc_root"

#include <m_log.h>
#include <m_malloc.h>
#include "m_gc_internal.h"

#define M_GC_RNODE_FLAGS (M_GC_BUF_FL_PERMANENT | M_GC_BUF_FL_PTR)
#define M_GC_RBUF_FLAGS  (M_GC_BUF_FL_PERMANENT | M_GC_BUF_FL_PTR)

M_Hash gc_root_hash;

static inline void*
gc_root_get_key (const M_HashNode *node)
{
	return m_node_value(node, M_GCRootNode, node);
}

static inline void
gc_root_free_node (void *ptr)
{
	m_gc_free_buf(m_node_value(ptr, M_GCRootNode, node),
				sizeof(M_GCRootNode), M_GC_RNODE_FLAGS);
}

static inline void*
gc_root_alloc_buf (size_t size)
{
	return m_gc_alloc_buf(size, M_GC_RBUF_FLAGS);
}

static inline void
gc_root_free_buf (void *ptr, size_t size)
{
	m_gc_free_buf(ptr, size, M_GC_RBUF_FLAGS);
}

/**Root object hash table functions.*/
static const M_HashOps
gc_root_hash_ops = {
get_key: gc_root_get_key,
kv:      m_ptr_hash_kv_func,
equal:   m_ptr_hash_equal_func,
free_node: gc_root_free_node,
alloc_buf: gc_root_alloc_buf,
free_buf:  gc_root_free_buf
};

void
gc_root_hash_startup (void)
{
	m_hash_init(&gc_root_hash);
}

void
gc_root_hash_shutdown (void)
{
	m_hash_deinit(&gc_root_hash, &gc_root_hash_ops);
}

void
m_gc_add_root (void *ptr)
{
	M_GCRootNode *rn;
	M_HashNode *node;
	uint32_t kv;

	assert(ptr);

	pthread_mutex_lock(&m_gc_lock);

	node = m_hash_lookup_with_kv(&gc_root_hash, ptr, &gc_root_hash_ops,
				&kv);
	if (node) {
		rn = m_node_value(node, M_GCRootNode, node);
		rn->ref ++;
	} else {
		rn = m_gc_alloc_buf(sizeof(M_GCRootNode), M_GC_RNODE_FLAGS);
		m_assert_alloc(rn);

		rn->ref = 1;
		rn->ptr = ptr;

		if (m_hash_resize(&gc_root_hash, &gc_root_hash_ops) != M_OK)
			m_assert_alloc(NULL);

		m_hash_insert_with_kv(&gc_root_hash, &rn->node, kv,
					&gc_root_hash_ops);
	}

	pthread_mutex_unlock(&m_gc_lock);
}

void
m_gc_remove_root (void *ptr)
{
	M_GCRootNode *rn;
	M_HashNode *node, **prev;

	assert(ptr);

	pthread_mutex_lock(&m_gc_lock);

	node = m_hash_lookup_with_prev(&gc_root_hash, ptr, &gc_root_hash_ops,
				&prev);
	if (node) {
		rn = m_node_value(node, M_GCRootNode, node);

		if (rn->ref == 1) {
			m_hash_remove_from_prev(&gc_root_hash, prev);
			m_gc_free_buf(rn, sizeof(M_GCRootNode), M_GC_RNODE_FLAGS);
		} else {
			rn->ref --;
		}
	}

	pthread_mutex_unlock(&m_gc_lock);
}

