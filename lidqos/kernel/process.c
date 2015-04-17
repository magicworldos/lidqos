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

void run_A()
{
	while (1)
	{
		__asm__ volatile("int $0x80");

//		__asm__ volatile(
//				"movw $0xf, %ax;"
//				"movw %ax, %ds;"
//				"movl $0x800, %eax;"
//				"movl %eax, %esi;"
//				"movb $0x8, %ds:(%esi);"
//		);
	}
}

void run_B()
{
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
	int size = 0x800;

	pcb_A = alloc_mm(sizeof(s_pcb));
	init_process(pcb_A);
	pcb_A->run_addr = alloc_mm(size);
	pcb_A->stack = alloc_mm(size);
	pcb_A->stack0 = alloc_mm(size);
	pcb_A->tss.eip = 0x0;
	pcb_A->tss.esp = size;
	pcb_A->tss.esp0 = (u32) pcb_A->stack0 + size;
	addr_to_gdt(LDT_TYPE_CS, (u32) pcb_A->run_addr, &(pcb_A->ldt[0]), GDT_G_BYTE, size);
	addr_to_gdt(LDT_TYPE_DS, (u32) pcb_A->stack, &(pcb_A->ldt[1]), GDT_G_BYTE, size);
	mmcopy(&run_A, pcb_A->run_addr, size);

	pcb_B = alloc_mm(sizeof(s_pcb));
	init_process(pcb_B);
	pcb_B->run_addr = alloc_mm(size);
	pcb_B->stack = alloc_mm(size);
	pcb_B->stack0 = alloc_mm(size);
	pcb_B->tss.eip = 0x0;
	pcb_B->tss.esp = size;
	pcb_B->tss.esp0 = (u32) pcb_B->stack0 + size;
	addr_to_gdt(LDT_TYPE_CS, (u32) pcb_B->run_addr, &(pcb_B->ldt[0]), GDT_G_BYTE, size);
	addr_to_gdt(LDT_TYPE_DS, (u32) pcb_B->stack, &(pcb_B->ldt[1]), GDT_G_BYTE, size);
	mmcopy(&run_B, pcb_B->run_addr, size);

	s_pcb *pcb = alloc_mm(sizeof(s_pcb));
	init_process(pcb);
	pcb->tss.eip = 0;
	pcb->tss.esp = 0;
	pcb->tss.esp0 = 0;
	pcb->ldt[0].lgdt = DEFAULT_LDT_CODE;
	pcb->ldt[1].lgdt = DEFAULT_LDT_DATA;
	pcb->ldt[2].lgdt = DEFAULT_LDT_DATA;
	pcb->ldt[3].lgdt = DEFAULT_LDT_DATA;

	//设置多任务的gdt描述符
	addr_to_gdt(GDT_TYPE_TSS, (u32) &pcb->tss, &gdts[4], GDT_G_BYTE, 0x68);
	addr_to_gdt(GDT_TYPE_LDT, (u32) &(pcb->ldt[0]), &gdts[5], GDT_G_BYTE, 0x10);
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
	pcb->tss.ds = USER_DATA_SEL;
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

	addr_to_gdt(GDT_TYPE_TSS, (u32) &pcb->tss, &gdts[4], GDT_G_BYTE, 0x68);
	addr_to_gdt(GDT_TYPE_LDT, (u32) &(pcb->ldt[0]), &gdts[5], GDT_G_BYTE, 0x80);

	call_tss();
}

#endif
