/*
 * sche.c
 * Copyright (C) Feb 10, 2014 by lidq
 *
 * 调度程序
 *  - 集中管理所有task的运行方式。
 *  - 采用多级反馈队列调度方式管理任务。
 *  - bug : 还没有解决关于tty切换时shell的阻塞时显示错误的问题
 */

#ifndef _SCHE_C_
#define _SCHE_C_

#include <kernel/sche.h>

int timer = 0;
//浮点寄存器数据临时保存区
char fpu_d[FPU_SIZE] =
{ 0xff, };

//全局描述符
extern s_gdt gdts[GDT_MAX_SIZE];

s_list *list_system = NULL;
s_list *list_service = NULL;
s_list *list_user = NULL;

s_list *list_wait = NULL;
s_list *list_stop = NULL;

s_list *pcb_current = NULL;
s_list *last_run = NULL;

s_list *list_pcb_all = NULL;

s_pcb *pcb_cur = NULL;

void schedule()
{
	s_list *list_header = list_user;

	if (list_header == NULL)
	{
		return;
	}

	s_list *list_run = list_header;
	s_pcb *pcb = (s_pcb *) (list_run->node);
	pcb_cur = pcb;
	if (list_header != last_run)
	{
		list_user = list_header2footer(list_user);

		addr_to_gdt(GDT_TYPE_TSS, (u32) &(pcb->tss), &gdts[4], GDT_G_BYTE, sizeof(s_tss) * 8);
		addr_to_gdt(GDT_TYPE_LDT, (u32) (pcb->ldt), &gdts[5], GDT_G_BYTE, sizeof(s_gdt) * 2 * 8);

		last_run = list_run;
		pcb_current = list_run;

		//在时钟中断时并没有切换ds和cr3寄存器
		//但是在call tss时cr3会被修改为tss中的cr3
		set_ds(0xf);
		call_tss();
	}
}

void pcb_insert(s_pcb *pcb)
{
	if (pcb == NULL)
	{
		return;
	}
	s_list *p_list = alloc_mm(sizeof(s_list));
	p_list->node = pcb;

	list_user = list_insert_node(list_user, p_list);

	s_list *pn_list = alloc_mm(sizeof(s_list));
	pn_list->node = pcb;
	list_pcb_all = list_insert_node(list_pcb_all, pn_list);
}
//
//void set_process_ready(s_pcb *pcb)
//{
//	if (pcb == NULL)
//	{
//		return;
//	}
//
//	s_list* p_list = get_process(pcb->process_id);
//
//	if (p_list == NULL)
//	{
//		return;
//	}
//
//	list_wait = list_remove_node(list_wait, p_list);
//
//	list_user = list_insert_node(list_user, p_list);
//}
//
//void set_process_wait(s_pcb *pcb)
//{
//	if (pcb == NULL)
//	{
//		return;
//	}
//
//	s_list* p_list = get_process(pcb->process_id);
//
//	if (p_list == NULL)
//	{
//		return;
//	}
//
//	list_user = list_remove_node(list_user, p_list);
//
//	list_wait = list_insert_node(list_wait, p_list);
//}
//
//void set_process_stop(s_list *p_list)
//{
//	if (p_list == NULL)
//	{
//		return;
//	}
//
//	s_pcb *pcb = p_list->node;
//
//	list_user = list_remove_node(list_user, p_list);
//
//	list_stop = list_insert_node(list_stop, p_list);
//}
//
//s_list* get_process(int process_id)
//{
//	s_list *p = list_user;
//	while (p != NULL)
//	{
//		s_pcb *pcb = p->node;
//		if (pcb->process_id == process_id)
//		{
//			return p;
//		}
//		p = p->next;
//	}
//
//	p = list_wait;
//	while (p != NULL)
//	{
//		s_pcb *pcb = p->node;
//		if (pcb->process_id == process_id)
//		{
//			return p;
//		}
//		p = p->next;
//	}
//
//	p = list_stop;
//	while (p != NULL)
//	{
//		s_pcb *pcb = p->node;
//		if (pcb->process_id == process_id)
//		{
//			return p;
//		}
//		p = p->next;
//	}
//	return NULL;
//}
//
//void free_process()
//{
//	while (list_stop != NULL)
//	{
//		s_list *p_list = list_stop;
//		s_pcb *pcb = p_list->node;
//		list_stop = list_stop->next;
//
////		free_mm((void*) pcb->program_addr, pcb->program_size);
////		free_mm((void*) pcb->stack_esp0, pcb->stack_size);
////		free_mm((void*) pcb->stack_esp, pcb->stack_size);
////		free_mm((void*) pcb->params, SHELL_CMD_LEN);
////		free_mm((void*) pcb->task, sizeof(s_task));
////		free_mm((void*) pcb, sizeof(s_pcb));
//		free_mm((void*) p_list, sizeof(s_list));
//	}
//}
//
///*
// * kill_task : 杀死任务
// *  - int task_id : 任务号
// * return : void
// */
//void kill_process(int process_id)
//{
//	s_list* pn = list_pcb_all;
//	s_list* fpn = pn;
//	while (pn != NULL)
//	{
//		s_pcb *pcb = (s_pcb *) (pn->node);
//		if (pcb->process_id == process_id)
//		{
//			//需要重新实现
//			//pcb_free_all(pcb->process_id);
//			list_pcb_all = list_remove_node(list_pcb_all, pn);
//			free_mm(pn, sizeof(s_list));
//			break;
//		}
//
//		if (fpn == pn)
//		{
//			pn = pn->next;
//		}
//		else
//		{
//			pn = pn->next;
//			fpn = fpn->next;
//		}
//	}
//
//	s_list* p = get_process(process_id);
//	set_process_stop(p);
//}

#endif
