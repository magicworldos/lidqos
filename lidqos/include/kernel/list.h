/*
 * list.h
 *
 *  Created on: Dec 4, 2014
 *      Author: lidq
 */

#ifndef _LIST_H_
#define _LIST_H_

typedef struct list_s
{
	void *node;
	struct list_s* next;
} s_list;

s_list* list_insert_node(s_list *list, s_list *p_list);

s_list* list_remove_node(s_list *list, s_list *p_list);

s_list* list_header2footer(s_list *list);

#endif /* LIST_H_ */
