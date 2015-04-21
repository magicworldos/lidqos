/*
 * task.c
 * Copyright (C) Feb 10, 2014 by lidq
 *
 * 多任务处理:
 *  - CPU的tts多任务切换功能
 */

#ifndef _PROCESS_C_
#define _PROCESS_C_

#include <kernel/process.h>

extern s_gdt gdts[GDT_MAX_SIZE];

s_pcb *pcb_A = NULL;
s_pcb *pcb_B = NULL;
s_pcb *pcb_current = NULL;
int timer = 0;

void run_A()
{
	char *p = (char *) 0xb8000;
	p += ((23 * 80 + 74)) * 2;
	int i = 33;
	while (1)
	{
		*p = i;
		if (++i >= 127)
		{
			i = 33;
		}
	}
}

void run_B()
{
//	char *p = (char *) 0xb8000;
//	p += ((23 * 80 + 76)) * 2;
//	int i = 64;
//	while (1)
//	{
//		*p = i;
//		if (++i >= 127)
//		{
//			i = 33;
//		}
//	}
	char *p = (char *) 0x812345;
	*p = 'A';
	while (1)
	{
	}
}

/*
 * install_task : 安装多任务
 * return : void
 */
void install_process()
{
	int pages = 0x4;
	int pagesize = 0x1000 * 0x4;
	pcb_A = alloc_page(pages, MM_SWAP_TYPE_CAN);
	init_process(pcb_A);
	pcb_A->pid = 1;
	pcb_A->tss.eip = (u32) &run_A;
	pcb_A->tss.esp = (u32) alloc_page(pages, MM_SWAP_TYPE_CAN) + pagesize;
	pcb_A->tss.esp0 = (u32) alloc_page(pages, MM_SWAP_TYPE_CAN) + pagesize;

	pcb_B = alloc_page(pages, MM_SWAP_TYPE_CAN);
	init_process(pcb_B);
	pcb_B->pid = 2;
	pcb_B->tss.eip = (u32) &run_B;
	pcb_B->tss.esp = (u32) alloc_page(pages, MM_SWAP_TYPE_CAN) + pagesize;
	pcb_B->tss.esp0 = (u32) alloc_page(pages, MM_SWAP_TYPE_CAN) + pagesize;

	s_pcb *pcb = alloc_page(pages, MM_SWAP_TYPE_CAN);
	init_process(pcb);
	pcb->tss.eip = 0;
	pcb->tss.esp = 0;
	pcb->tss.esp0 = 0;

	addr_to_gdt(GDT_TYPE_TSS, (u32) &pcb->tss, &gdts[4], GDT_G_BYTE, sizeof(s_tss) * 8);
	addr_to_gdt(GDT_TYPE_LDT, (u32) pcb->ldt, &gdts[5], GDT_G_BYTE, sizeof(s_gdt) * 2 * 8);

	//载入tss和ldt
	load_tss(GDT_INDEX_TSS);
	load_ldt(GDT_INDEX_LDT);
}

/*
 * create_task : 创建tts任务
 *  - int type : tts任务类型TASK_TYPE_NOR、TASK_TYPE_SPE
 * return : void
 */
void init_process(s_pcb *pcb)
{
	//s_tss
	pcb->tss.back_link = 0;
	pcb->tss.ss0 = GDT_INDEX_KERNEL_DS;
	pcb->tss.esp1 = 0;
	pcb->tss.ss1 = 0;
	pcb->tss.esp2 = 0;
	pcb->tss.ss2 = 0;
	//(0x300000)
	pcb->tss.cr3 = (0x300000);
	pcb->tss.eflags = 0x3202;
	pcb->tss.eax = 0;
	pcb->tss.ecx = 0;
	pcb->tss.edx = 0;
	pcb->tss.ebx = 0;
	pcb->tss.ebp = 0;
	pcb->tss.esi = 0;
	pcb->tss.edi = 0;
	pcb->tss.es = USER_DATA_SEL;
	pcb->tss.cs = USER_CODE_SEL;
	pcb->tss.ss = USER_DATA_SEL;
	pcb->tss.ds = USER_DATA_SEL;
	pcb->tss.fs = USER_DATA_SEL;
	pcb->tss.gs = USER_DATA_SEL;
	pcb->tss.ldt = GDT_INDEX_LDT;
	pcb->tss.trace_bitmap = 0;

	//设置多任务的gdt描述符
	addr_to_gdt(LDT_TYPE_CS, 0, &(pcb->ldt[0]), GDT_G_KB, 0xfffff);
	addr_to_gdt(LDT_TYPE_DS, 0, &(pcb->ldt[1]), GDT_G_KB, 0xfffff);
}

void schedule()
{
	if (timer++ % 2 == 0)
	{
		pcb_current = pcb_A;
	}
	else
	{
		pcb_current = pcb_B;
	}

	addr_to_gdt(GDT_TYPE_TSS, (u32) &pcb_current->tss, &gdts[4], GDT_G_BYTE, sizeof(s_tss) * 8);
	addr_to_gdt(GDT_TYPE_LDT, (u32) pcb_current->ldt, &gdts[5], GDT_G_BYTE, sizeof(s_gdt) * 2 * 8);

	call_tss();
}

#endif
