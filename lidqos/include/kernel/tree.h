/*
 * tree.h
 *
 *  Created on: Feb 12, 2015
 *      Author: lidq
 */

#ifndef _INCLUDE_KERNEL_TREE_H_
#define _INCLUDE_KERNEL_TREE_H_

#include <kernel/typedef.h>

typedef struct s_tree
{
	int id;
	void *node;
	struct s_tree *parent;
	struct s_tree *child;
	struct s_tree *next;
} s_tree;

void tree_init_node(s_tree *tree, int id);

s_tree* tree_find_depth(s_tree *tree, int id);

void tree_insert_node(s_tree *tree, int id, s_tree *p_tree);

s_tree* tree_remove_node(s_tree *tree, int id);

void tree_depth_display(s_tree *tree);

//int tree_test(void)

#endif /* INCLUDE_KERNEL_TREE_H_ */
