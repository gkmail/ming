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

#define M_LOG_TAG "rbttest"

#include <ming.h>

typedef struct {
	M_RBNode node;
	int      i;
} IntNode;

static inline void*
int_get_key (const M_RBNode *node)
{
	return M_SIZE_TO_PTR(m_node_value(node, IntNode, node)->i);
}

static const M_RBTreeOps
int_ops = {
get_key:   int_get_key,
cmp:       m_int_cmp_func,
free_node: m_free
};

static void
insert_remove_test (void)
{
#define LOOP_COUNT 1000
#define NUM_COUNT  1000
	M_RBTree tree;
	M_RBNode *node, *parent, **pos;
	IntNode *in;
	int i;
	int count = NUM_COUNT;
	int loop = LOOP_COUNT;
	int nums[NUM_COUNT];

	M_INFO("insert remove test begin");

	m_rbt_init(&tree);

	for (i = 0; i < count; i ++) {
		nums[i] = i;
	}

	while (loop --) {
		for (i = 0; i < count; i ++) {
			int x = rand() % count;

			M_EXCHANGE(nums[i], nums[x]);
		}

		for (i = 0; i < count; i ++) {
			node = m_rbt_lookup_insert(&tree, M_SIZE_TO_PTR(nums[i]),
						&int_ops, &parent, &pos);
			if (node) {
				M_ERROR("lookup error");
			} else {
				in = M_NEW(IntNode, 1);
				in->i = nums[i];

				m_rbt_insert(&tree, parent, pos, &in->node);
			}
		}

		for (i = 0; i < count; i ++) {
			int x = rand() % count;

			M_EXCHANGE(nums[i], nums[x]);
		}

		for (i = 0; i < count; i ++) {
			node = m_rbt_lookup(&tree, M_SIZE_TO_PTR(nums[i]), &int_ops);
			if (!node) {
				M_ERROR("lookup error");
			}

			m_rbt_remove(&tree, node);
			m_free(node);
		}
	}

	m_rbt_deinit(&tree, &int_ops);

	M_INFO("insert remove test end");
}

static void
traverse_test (void)
{
	M_RBTree tree;
	M_RBNode *node, *nnode, *parent, **pos;
	IntNode *in, *nin;
	int i;
	int count = NUM_COUNT;

	M_INFO("traverse test begin");

	m_rbt_init(&tree);

	for (i = 0; i < count; i ++) {
		node = m_rbt_lookup_insert(&tree, M_SIZE_TO_PTR(i), &int_ops,
					&parent, &pos);
		if (node) {
			M_ERROR("lookup error");
		} else {
			in = M_NEW(IntNode, 1);
			in->i = i;

			m_rbt_insert(&tree, parent, pos, &in->node);
		}
	}

	i = 0;
	m_rbt_foreach(node, &tree) {
		in = m_node_value(node, IntNode, node);
		if (in->i != i) {
			M_ERROR("traverse error");
		}
		i ++;
	}
	if (i != count) {
		M_ERROR("traverse error");
	}

	i = 0;
	m_rbt_foreach_value(in, &tree, node) {
		if (in->i != i) {
			M_ERROR("traverse error");
		}
		i ++;
	}
	if (i != count) {
		M_ERROR("traverse error");
	}

	i = count;
	m_rbt_foreach_r(node, &tree) {
		in = m_node_value(node, IntNode, node);
		if (in->i != i - 1) {
			M_ERROR("traverse error");
		}
		i --;
	}
	if (i != 0) {
		M_ERROR("traverse error");
	}

	i = count;
	m_rbt_foreach_value_r(in, &tree, node) {
		if (in->i != i - 1) {
			M_ERROR("traverse error");
		}
		i --;
	}
	if (i != 0) {
		M_ERROR("traverse error");
	}

	m_rbt_foreach_safe(node, nnode, &tree) {
		in = m_node_value(node, IntNode, node);
		if ((in->i % 10) == 0) {
			m_rbt_remove(&tree, node);
			m_free(in);
		}
	}

	m_rbt_foreach_safe_r(node, nnode, &tree) {
		in = m_node_value(node, IntNode, node);
		if ((in->i % 10) == 1) {
			m_rbt_remove(&tree, node);
			m_free(in);
		}
	}

	m_rbt_foreach_value_safe(in, nin, &tree, node) {
		if ((in->i % 10) == 2) {
			m_rbt_remove(&tree, &in->node);
			m_free(in);
		}
	}

	m_rbt_foreach_value_safe_r(in, nin, &tree, node) {
		if ((in->i % 10) == 3) {
			m_rbt_remove(&tree, &in->node);
			m_free(in);
		}
	}

	for (i = 0; i < count; i ++) {
		node = m_rbt_lookup(&tree, M_SIZE_TO_PTR(i), &int_ops);
		if (node && ((i % 10) < 4)) {
			M_ERROR("remove error");
		}
		if (!node && ((i % 10) > 3)) {
			M_ERROR("remove error");
		}
	}

	m_rbt_deinit(&tree, &int_ops);

	M_INFO("traverse test end");
}

int
main (int argc, char **argv)
{
	m_startup();

	insert_remove_test();
	traverse_test();

	return 0;
}

