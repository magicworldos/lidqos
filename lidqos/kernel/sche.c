/*
 * sche.c
 * Copyright (C) Feb 10, 2014 by lidq
 *
 * 调度程序
 *  - 集中管理所有task的运行方式。
 *  - 采用多级反馈链表调度方式管理任务。
 *  - bug : 还没有解决关于tty切换时shell的阻塞时显示错误的问题
 */

#include <kernel/sche.h>

//全局描述符
extern s_gdt gdts[GDT_MAX_SIZE];

//进程链表
s_list *list_pcb = NULL;
//等待链表sleep
s_list *list_pcb_sleep = NULL;
//停止链表stop
s_list *list_pcb_stop = NULL;

//当前正在运行的进程
s_pcb *pcb_cur = NULL;
//上一次运行的进程
s_pcb *pcb_last_run = NULL;

//等待按键
s_list *list_pcb_wait_key = NULL;

/*
 * 进程调度，目前只使用平均时间片轮转的算法
 */
void schedule()
{
	//取得链表头
	s_list *list_header = list_pcb;
	if (list_header == NULL)
	{
		return;
	}
	//show_list();
	//取得链表头的进程
	pcb_cur = (s_pcb *) (list_header->node);
	//将链表头移动到链表尾，链表
	list_pcb = list_header2footer(list_pcb);
	/*
	 * 如果当前进程不是上一次运行的进程，说明链表中有两个或两个以上的进程
	 * 链表中只有一个进程，不需要切换。
	 * 链表中有两个或两个以上的进程，需要切换。
	 */
	if (pcb_cur != pcb_last_run)
	{
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
 * 在每个时间片中都执行此函数，将等待链表中的每一个进程的等待时间都减少一个时间片周期
 * 并判断每个进程是否已“时间到”，如果时间已到，则将其移出等待链表并加入运行链表
 */
void list_sleep_change()
{
	s_list* p = list_pcb_sleep;
	while (p != NULL)
	{
		s_pcb *pcb = (s_pcb *) p->node;
		//减少一个时钟周期
		pcb->sleep_ms -= CLOCK_TIMER;
		p = p->next;
		if (pcb->sleep_ms < 0)
		{
			pcb->sleep_ms = 0;
		}
		//如果“时间到”
		if (pcb->sleep_ms == 0)
		{
			s_list *list_node = NULL;
			//从运行链表中移出此进程
			list_pcb_sleep = list_remove_node(list_pcb_sleep, pcb, &list_node);
			//加入到执行链表
			list_pcb = list_insert_node(list_pcb, list_node);
		}
	}
}

/*
 * 将进程加入到链表中
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

/*
 * 将pcb移出到等待链表中
 */
void pcb_sleep(s_pcb *pcb, int ms)
{
	if (pcb == NULL)
	{
		return;
	}

	if (ms <= 0)
	{
		return;
	}

	//设置等待毫秒
	pcb->sleep_ms = ms;
	//链表节点
	s_list *list_node = NULL;
	//从运行链表中移出此进程
	list_pcb = list_remove_node(list_pcb, pcb, &list_node);
	//加入到等待链表
	list_pcb_sleep = list_insert_node(list_pcb_sleep, list_node);
	//因为当前进程就是调用sleep中断的进程，为了让其等待要执行一次调度
	schedule();

}

int pcb_sem_P(s_pcb *pcb, s_sem *sem)
{
	if (pcb == NULL)
	{
		return 0;
	}

	if (sem->value > 0)
	{
		sem->value--;
		return 1;
	}
	pcb->tss.eip -= 2;
	//链表节点
	s_list *list_node = NULL;
	//从运行链表中移出此进程
	list_pcb = list_remove_node(list_pcb, pcb, &list_node);
	//加入到等待链表
	sem->list_block = list_insert_node(sem->list_block, list_node);

	return 0;
}

int pcb_sem_V(s_pcb *pcb, s_sem *sem)
{
	if (pcb == NULL)
	{
		return 0;
	}
	//信号量加1
	sem->value++;
	s_list *list_node = NULL;
	if (sem->list_block != NULL)
	{
		s_list *p = (s_list *) sem->list_block;
		s_pcb *pcb_wakeup = (s_pcb *) p->node;
		//从运行链表中移出此进程
		sem->list_block = list_remove_node(sem->list_block, pcb_wakeup, &list_node);
		//加入到执行链表
		list_pcb = list_insert_node(list_pcb, list_node);
	}
	return 1;
}

/*
 * 停止进程
 */
void pcb_stop(s_pcb *pcb)
{
	s_list *list_node = NULL;
	//从运行链表中移出此进程
	list_pcb = list_remove_node(list_pcb, pcb, &list_node);
	//加入到停止链表
	list_pcb_stop = list_insert_node(list_pcb_stop, list_node);
	//执行一次调度，跳过当前进程
	schedule();
}

/*
 * 释放已停止的pcb的资源
 */
void pcb_free()
{
	s_list *p = list_pcb_stop;
	while (p != NULL)
	{
		s_list *pf = p;
		p = p->next;

		s_pcb *pcb = (s_pcb *) pf->node;
		if (pcb->children == NULL)
		{
			s_list *list_node = NULL;
			list_pcb_stop = list_remove_node(list_pcb_stop, pcb, &list_node);
			if (pcb->parent != NULL)
			{
				s_pcb *parent = pcb->parent;
				s_list *list_n = NULL;
				//把当前pcb从其父进程的children中移出
				parent->children = list_remove_node(parent->children, pcb, &list_n);
			}
			u32 pid = pcb->process_id;
			free_page_by_pid(pid);
			free_mm(pf, sizeof(s_list));
		}
	}
}

void pcb_wait_key(s_pcb *pcb)
{
	s_list *list_node = NULL;
	list_pcb = list_remove_node(list_pcb, pcb, &list_node);
	list_pcb_wait_key = list_insert_node(list_pcb_wait_key, list_node);
}

void pcb_wakeup_key()
{
	s_pcb *pcb = NULL;
	if (list_pcb_wait_key != NULL)
	{
		pcb = (s_pcb *) list_pcb_wait_key->node;
	}
	s_list *list_node = NULL;
	list_pcb_wait_key = list_remove_node(list_pcb_wait_key, pcb, &list_node);
	list_pcb = list_insert_node(list_pcb, list_node);
}
