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

int timer = 0;
void run()
{
	while (1)
	{
	}
}
void run_A()
{
//	int i = 0;
//	while (1)
//	{
//		__asm__(
//				"int $0x80		\n\r"
//		);
//	__asm__(
//			"movw $0x1f, %ax		\n\r"
//			"movw %ax, %ds			\n\r"
//	);
//	}
//	__asm__ volatile("hlt");

	while (1)
	{
		__asm__ volatile("int $0x80");
	}
//	char *p = (char *) 0xb8000;
//	p += ((23 * 80 + 78)) * 2;
//	int i = 64;
//	while (1)
//	{
//		*p = i;
//		if (++i >= 127)
//		{
//			i = 33;
//		}
//	}
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
	while (1)
	{
		__asm__ volatile("int $0x81");
	}
}

/*
 * install_task : 安装多任务
 * return : void
 */
void install_process()
{
	pcb_A = alloc_mm(sizeof(s_pcb));
	init_process(pcb_A);
	pcb_A->run_addr = alloc_mm(0x800);
	pcb_A->ds_addr = alloc_mm(0x800);
	pcb_A->stack = alloc_mm(0x800);
	pcb_A->stack0 = alloc_mm(0x800);
	pcb_A->tss.eip = 0x0;
	pcb_A->tss.esp = 0x800;
	pcb_A->tss.esp0 = (u32) pcb_A->stack0 + 0x800;
	addr_to_gdt_or_ldt((u32) pcb_A->run_addr, (s_gdt*) &(pcb_A->ldt[0]), LDT_TYPE_CS);
	addr_to_gdt_or_ldt((u32) pcb_A->stack, (s_gdt*) &(pcb_A->ldt[1]), LDT_TYPE_DS);
	addr_to_gdt_or_ldt((u32) 0, (s_gdt*) &(pcb_A->ldt[2]), LDT_TYPE_DS);
	addr_to_gdt_or_ldt((u32) 0, (s_gdt*) &(pcb_A->ldt[3]), LDT_TYPE_DS);
//	pcb_A->ldt[0] = DEFAULT_LDT_CODE;
//	pcb_A->ldt[1] = DEFAULT_LDT_DATA;
//	pcb_A->ldt[2] = DEFAULT_LDT_DATA;
//	pcb_A->ldt[3] = DEFAULT_LDT_DATA;
	mmcopy(&run_A, pcb_A->run_addr, 0x800);

	pcb_B = alloc_mm(sizeof(s_pcb));
	init_process(pcb_B);
	pcb_B->run_addr = alloc_mm(0x800);
	pcb_B->ds_addr = alloc_mm(0x800);
	pcb_B->stack = alloc_mm(0x800);
	pcb_B->stack0 = alloc_mm(0x800);
	pcb_B->tss.eip = 0x0;
	pcb_B->tss.esp = 0x800;
	pcb_B->tss.esp0 = (u32) pcb_B->stack0 + 0x800;
	addr_to_gdt_or_ldt((u32) pcb_B->run_addr, (s_gdt*) &(pcb_B->ldt[0]), LDT_TYPE_CS);
	addr_to_gdt_or_ldt((u32) pcb_B->stack, (s_gdt*) &(pcb_B->ldt[1]), LDT_TYPE_DS);
	addr_to_gdt_or_ldt((u32) 0, (s_gdt*) &(pcb_B->ldt[2]), LDT_TYPE_DS);
	addr_to_gdt_or_ldt((u32) 0, (s_gdt*) &(pcb_B->ldt[3]), LDT_TYPE_DS);
//	pcb_B->ldt[0] = DEFAULT_LDT_CODE;
//	pcb_B->ldt[1] = DEFAULT_LDT_DATA;
//	pcb_B->ldt[2] = DEFAULT_LDT_DATA;
//	pcb_B->ldt[3] = DEFAULT_LDT_DATA;
	mmcopy(&run_B, pcb_B->run_addr, 0x800);

	s_pcb *pcb = alloc_mm(sizeof(s_pcb));
	init_process(pcb);
	pcb->tss.eip = 0;
	pcb->tss.esp = 0;
	pcb->tss.esp0 = 0;
	pcb->ldt[0] = DEFAULT_LDT_CODE;
	pcb->ldt[1] = DEFAULT_LDT_DATA;
	pcb->ldt[2] = DEFAULT_LDT_DATA;
	pcb->ldt[3] = DEFAULT_LDT_DATA;

	//设置多任务的gdt描述符
	addr_to_gdt_or_ldt((u32) &pcb->tss, &gdts[4], GDT_TYPE_TSS);
	addr_to_gdt_or_ldt((u32) &(pcb->ldt[0]), &gdts[5], GDT_TYPE_LDT);

	//载入tss和ldt
	load_tss(GDT_INDEX_TSS);
	load_ldt(GDT_INDEX_LDT);

//	addr_to_gdt_or_ldt((u32) &(pcb_A->tss), &gdts[4], GDT_TYPE_TSS);
//	addr_to_gdt_or_ldt((u32) &(pcb_A->ldt[0]), &gdts[5], GDT_TYPE_LDT);
//	call_tss();
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
	pcb->tss.cr3 = 0;
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
	pcb->tss.ds = USER_DATA_SEL2;
	pcb->tss.fs = USER_DATA_SEL;
	pcb->tss.gs = USER_DATA_SEL;
	pcb->tss.ldt = GDT_INDEX_LDT;
	pcb->tss.trace_bitmap = 0;
}

void schedule()
{
	s_pcb *pcb = NULL;

	if (timer++ % 2 == 0)
	{
		pcb = pcb_A;
	}
	else
	{
		pcb = pcb_B;
	}

	addr_to_gdt_or_ldt((u32) &(pcb->tss), &gdts[4], GDT_TYPE_TSS);
	addr_to_gdt_or_ldt((u32) &(pcb->ldt[0]), &gdts[5], GDT_TYPE_LDT);
	//load_ldt(GDT_INDEX_LDT);

//	call_tss();
//	__asm__(
//			"movw $0x17, %ax		\n\r"
//			"movw %ax, %ds			\n\r"
//	);
	__asm__ volatile("ljmp $0x20, $0");
}

#endif
