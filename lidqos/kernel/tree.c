/*
 * tree.c
 *
 *  Created on: Feb 12, 2015
 *      Author: lidq
 */

#ifndef _SOURCE_KERNEL_TREE_C_
#define _SOURCE_KERNEL_TREE_C_

#include <kernel/tree.h>

void tree_init_node(s_tree *tree, int id)
{
	tree->id = id;
	tree->node = NULL;
	tree->parent = NULL;
	tree->child = NULL;
	tree->next = NULL;
}

s_tree* tree_find_depth(s_tree *tree, int id)
{
	s_tree *p = tree;

	while (p != NULL)
	{
		if (p->id == id)
		{
			return p;
		}

		if (p->child != NULL)
		{
			if (p->child->id == id)
			{
				return p->child;
			}

			s_tree *ret = tree_find_depth(p->child, id);
			if (ret != NULL)
			{
				return ret;
			}
		}
		p = p->next;
	}
	return NULL;
}

void tree_insert_node(s_tree *tree, int id, s_tree *p_tree)
{
	s_tree *p_parent = tree_find_depth(tree, id);
	s_tree *p = p_parent;
	if (p != NULL)
	{
		if (p->child == NULL)
		{
			p->child = p_tree;
			p_tree->parent = p_parent;
			return;
		}

		p = p->child;

		while (p->next != NULL)
		{
			p = p->next;
		}

		p->next = p_tree;
		p_tree->parent = p_parent;

		return;
	}
}

s_tree * tree_remove_node(s_tree *tree, int id)
{
	s_tree *p = tree_find_depth(tree, id);

	if (p == NULL)
	{
		//printf("not find this node.\n");
		return NULL;
	}

	if (p->parent == NULL)
	{
		//printf("can not remove the root node.\n");
		return NULL;
	}

	if (p->child != NULL)
	{
		//printf("this node is not empty.\n");
		return NULL;
	}

	s_tree *t = p->parent;
	//p is first child node
	if (p == t->child)
	{
		t->child = p->next;
		p->next = NULL;
		return p;
	}

	t = t->child;
	while (t->next != p)
	{
		t = t->next;
	}
	t->next = p->next;
	p->next = NULL;
	return p;
}

void tree_depth_display(s_tree *tree)
{
	s_tree *p = tree;
	while (p != NULL)
	{
		//printf("%d ", p->id);
		if (p->child != NULL)
		{
			tree_depth_display(p->child);
		}
		p = p->next;
	}
}

//int tree_test(void)
//{
//	int next_id = 0;
//	s_tree *root = malloc(sizeof(s_tree));
//	tree_init_node(root, next_id++);
//
//	s_tree *t1 = malloc(sizeof(s_tree));
//	tree_init_node(t1, next_id++);
//	tree_insert_node(root, 0, t1);
//
//	s_tree *t2 = malloc(sizeof(s_tree));
//	tree_init_node(t2, next_id++);
//	tree_insert_node(root, 0, t2);
//
//	s_tree *t3 = malloc(sizeof(s_tree));
//	tree_init_node(t3, next_id++);
//	tree_insert_node(root, 1, t3);
//
//	s_tree *t4 = malloc(sizeof(s_tree));
//	tree_init_node(t4, next_id++);
//	tree_insert_node(root, 2, t4);
//
//	s_tree *t5 = malloc(sizeof(s_tree));
//	tree_init_node(t5, next_id++);
//	tree_insert_node(root, 2, t5);
//
//	s_tree *t6 = malloc(sizeof(s_tree));
//	tree_init_node(t6, next_id++);
//	tree_insert_node(root, 4, t6);
//
//	s_tree *t7 = malloc(sizeof(s_tree));
//	tree_init_node(t7, next_id++);
//	tree_insert_node(root, 4, t7);
//
//	s_tree *t8 = malloc(sizeof(s_tree));
//	tree_init_node(t8, next_id++);
//	tree_insert_node(root, 7, t8);
//
//	s_tree *t9 = malloc(sizeof(s_tree));
//	tree_init_node(t9, next_id++);
//	tree_insert_node(root, 5, t9);
//
//	s_tree *t10 = malloc(sizeof(s_tree));
//	tree_init_node(t10, next_id++);
//	tree_insert_node(root, 5, t10);
//
//	s_tree *t11 = malloc(sizeof(s_tree));
//	tree_init_node(t11, next_id++);
//	tree_insert_node(root, 4, t11);
//
//	s_tree *t12 = malloc(sizeof(s_tree));
//	tree_init_node(t12, next_id++);
//	tree_insert_node(root, 4, t12);
//
//	s_tree *t13 = malloc(sizeof(s_tree));
//	tree_init_node(t13, next_id++);
//	tree_insert_node(root, 12, t13);
//
//	s_tree *t14 = malloc(sizeof(s_tree));
//	tree_init_node(t14, next_id++);
//	tree_insert_node(root, 12, t14);
//
//	s_tree *t15 = malloc(sizeof(s_tree));
//	tree_init_node(t15, next_id++);
//	tree_insert_node(root, 12, t15);
//
//	for (int i = 0; i < 16; i++)
//	{
//		s_tree *r = tree_find_depth(root, i);
//		if (r == NULL)
//		{
//			printf("not find.");
//		}
//		else
//		{
//			printf("find %d\n", r->id);
//		}
//	}
//
//	printf("-------------------------------\n");
//	tree_depth_display(root);
//
//	printf("\n-------------------------------\n");
//	s_tree *del = tree_remove_node(root, 11);
//	free(del);
//	tree_depth_display(root);
//
//	return 0;
//}

#endif
