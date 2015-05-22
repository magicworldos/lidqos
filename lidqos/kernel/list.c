/*
 * list.c
 *
 *  Created on: Dec 4, 2014
 *      Author: lidq
 */

#include <kernel/list.h>

/*
 * 为列表插入一个新节点
 */
s_list* list_insert_node(s_list *list, s_list *p_list)
{
	if (p_list == NULL)
	{
		return list;
	}

	p_list->next = NULL;

	s_list* header = list;
	if (header == NULL)
	{
		header = p_list;
	}
	else
	{
		p_list->next = header;
		header = p_list;
	}
	return header;
}

/*
 * 移除一个节点
 */
s_list* list_remove_node(s_list *list, s_list *p_list)
{
	if (p_list == NULL)
	{
		return list;
	}

	s_list* header = list;
	s_list* p = header;
	s_list* fp = p;
	while (p != NULL)
	{
		if (p == p_list)
		{
			if (p == fp)
			{
				header = p->next;
				p->next = NULL;
				return header;
			}
			else
			{
				if (p->next == NULL)
				{
					fp->next = NULL;
					return header;
				}
				else
				{
					fp->next = p->next;
					p->next = NULL;
					return header;
				}
			}
		}

		if (p == fp)
		{
			p = p->next;
		}
		else
		{
			p = p->next;
			fp = fp->next;
		}
	}
	return header;
}

/*
 * 将链表的头节点移动到尾部，即循环队列
 */
s_list* list_header2footer(s_list *list)
{
	s_list* header = list;

	if (header == NULL)
	{
		return NULL;
	}

	if (header->next == NULL)
	{
		return header;
	}

	s_list *fst = header;
	s_list *p = header;
	header = header->next;

	while (p->next != NULL)
	{
		p = p->next;
	}
	p->next = fst;
	fst->next = NULL;

	return header;
}
