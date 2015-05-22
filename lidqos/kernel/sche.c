/*
 * sche.c
 * Copyright (C) Feb 10, 2014 by lidq
 *
 * 调度程序
 *  - 集中管理所有task的运行方式。
 *  - 采用多级反馈队列调度方式管理任务。
 *  - bug : 还没有解决关于tty切换时shell的阻塞时显示错误的问题
 */

#include <kernel/sche.h>

//全局描述符
extern s_gdt gdts[GDT_MAX_SIZE];

//进程队列
s_list *list_pcb = NULL;
//当前正在运行的进程
s_pcb *pcb_cur = NULL;
//上一次运行的进程
s_pcb *pcb_last_run = NULL;

/*
 * 进程调度，目前只使用平均时间片轮转的算法
 */
void schedule()
{
	//取得队列头
	s_list *list_header = list_pcb;
	if (list_header == NULL)
	{
		return;
	}

	//取得队列头的进程
	pcb_cur = (s_pcb *) (list_header->node);

	/*
	 * 如果当前进程不是上一次运行的进程，说明队列中有两个或两个以上的进程
	 * 队列中只有一个进程，不需要切换。
	 * 队列中有两个或两个以上的进程，需要切换。
	 */
	if (pcb_cur != pcb_last_run)
	{
		//将队列头移动到队列尾，循环队列
		list_pcb = list_header2footer(list_pcb);

		//设置tss和ldt
		addr_to_gdt(GDT_TYPE_TSS, (u32) &(pcb_cur->tss), &gdts[4], GDT_G_BYTE, sizeof(s_tss) * 8);
		addr_to_gdt(GDT_TYPE_LDT, (u32) (pcb_cur->ldt), &gdts[5], GDT_G_BYTE, sizeof(s_gdt) * 2 * 8);

		//将上一次运行进程设置为当前进程
		pcb_last_run = pcb_cur;

		//在时钟中断时并没有切换ds和cr3寄存器
		//但是在call tss时cr3会被修改为tss中的cr3
		set_ds(0xf);

		//切换进程
		call_tss();
	}
}

/*
 * 将进程加入到队列中
 */
void pcb_insert(s_pcb *pcb)
{
	if (pcb == NULL)
	{
		return;
	}

	s_list *p_list = alloc_mm(sizeof(s_list));
	p_list->node = pcb;
	list_pcb = list_insert_node(list_pcb, p_list);
}
