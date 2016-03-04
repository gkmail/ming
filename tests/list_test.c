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

#define M_LOG_TAG "listtest"

#include <ming.h>

typedef struct {
	M_List node;
	int    i;
} IntNode;

static void
add_remove_test (void)
{
	M_List head;
	IntNode *in, *nin;
	int i;
	int count = 1024*1024;

	M_INFO("add remove test begin");

	m_list_init(&head);

	if (!m_list_empty(&head)) {
		M_ERROR("empty test error");
	}

	for (i = 0; i < count; i ++) {
		in = M_NEW(IntNode, 1);
		in->i = i;

		m_list_append(&head, &in->node);
	}

	if (m_list_empty(&head)) {
		M_ERROR("empty test error");
	}

	i = 0;
	m_list_foreach_value_safe(in, nin, &head, node) {
		if (in->i != i) {
			M_ERROR("node value error");
		}
		m_list_remove(&in->node);
		m_free(in);

		i ++;
	}

	for (i = 0; i < count; i ++) {
		in = M_NEW(IntNode, 1);
		in->i = i;

		m_list_prepend(&head, &in->node);
	}

	i = 0;
	m_list_foreach_value_safe_r(in, nin, &head, node) {
		if (in->i != i) {
			M_ERROR("node value error");
		}
		m_list_remove(&in->node);
		m_free(in);

		i ++;
	}

	M_INFO("add remove test end");
}

static void
traverse_test (void)
{
	M_List head;
	int i;
	int count = 1024*1024;
	IntNode *in, *nin;
	M_List *node, *nnode;

	M_INFO("traverse test begin");

	m_list_init(&head);

	for (i = 0; i < count; i ++) {
		in = M_NEW(IntNode, 1);
		in->i = i;

		m_list_append(&head, &in->node);
	}

	i = 0;
	m_list_foreach(node, &head) {
		in = m_list_value(node, IntNode, node);
		if (in->i != i){
			M_ERROR("traverse error");
		}
		i ++;
	}
	if (i != count)
		M_ERROR("traverse error");

	i = 0;
	m_list_foreach_value(in, &head, node) {
		if (in->i != i){
			M_ERROR("traverse error");
		}
		i ++;
	}
	if (i != count)
		M_ERROR("traverse error");


	i = count;
	m_list_foreach_r(node, &head) {
		in = m_list_value(node, IntNode, node);
		if (in->i != i - 1){
			M_ERROR("traverse error");
		}
		i --;
	}
	if (i != 0)
		M_ERROR("traverse error");

	i = count;
	m_list_foreach_value_r(in, &head, node) {
		if (in->i != i - 1){
			M_ERROR("traverse error");
		}
		i --;
	}
	if (i != 0)
		M_ERROR("traverse error");

	m_list_foreach_safe(node, nnode, &head) {
		in = m_list_value(node, IntNode, node);
		if ((in->i % 10) == 0) {
			m_list_remove(node);
			m_free(node);
		}
	}

	m_list_foreach_safe_r(node, nnode, &head) {
		in = m_list_value(node, IntNode, node);
		if ((in->i % 10) == 1) {
			m_list_remove(node);
			m_free(node);
		}
	}

	m_list_foreach_value_safe(in, nin, &head, node) {
		if ((in->i % 10) == 2) {
			m_list_remove(&in->node);
			m_free(in);
		}
	}

	m_list_foreach_value_safe_r(in, nin, &head, node) {
		if ((in->i % 10) == 3) {
			m_list_remove(&in->node);
			m_free(in);
		}
	}

	i = 4;
	m_list_foreach_value_safe(in, nin, &head, node) {
		if (in->i != i) {
			M_ERROR("safe traverse error");
		}

		i ++;
		if ((i % 10) == 0) {
			i += 4;
		}

		m_list_remove(&in->node);
		m_free(in);
	}

	M_INFO("traverse test end");
}

typedef struct {
	M_SList node;
	int     i;
} SIntNode;

static void
slist_test (void)
{
	M_SList head, *node, *nnode;
	SIntNode *in, *nin;
	int i, count = 1024*1024;

	M_INFO("slist test begin");

	m_slist_init(&head);

	for (i = 0; i < count; i ++) {
		in = M_NEW(SIntNode, 1);
		in->i = i;

		m_slist_push(&head, &in->node);
	}

	i = count;
	m_slist_foreach(node, &head) {
		in = m_slist_value(node, SIntNode, node);
		if (in->i != i - 1) {
			M_ERROR("slist value error");
		}
		i --;
	}
	if (i != 0)
		M_ERROR("slist traverse error");


	i = count;
	m_slist_foreach_value(in, &head, node) {
		if (in->i != i - 1) {
			M_ERROR("slist value error");
		}
		i --;
	}
	if (i != 0)
		M_ERROR("slist traverse error");

	for (i = 0; i < count; i ++) {
		node = m_slist_pop(&head);
		if (!node) {
			M_ERROR("pop error");
		} else {
			in = m_slist_value(node, SIntNode, node);
			if (in->i != count - i - 1) {
				M_ERROR("node value error");
			}

			m_free(node);
		}
	}

	m_slist_foreach_safe(node, nnode, &head) {
		m_free(node);
	}

	M_INFO("slist test end");
}

int
main (int argc, char **argv)
{
	m_startup();

	add_remove_test();
	traverse_test();
	slist_test();

	return 0;
}

