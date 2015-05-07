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
	char *pT = (char *) 0x10000000;
	*pT = 'A';

	char *p = (char *) 0xb8000;
	p += ((23 * 80 + 74)) * 2;
	while (1)
	{
		//char *pT2 = (char *) 0x10000002;
		//*pT2 = 'S';
		*p = *pT;
	}

//	char *p = (char *) 0xb8000;
//	p += ((23 * 80 + 74)) * 2;
//	int i = 33;
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
	char *pT = (char *) 0x10001000;
	*pT = 'B';

	char *p = (char *) 0xb8000;
	p += ((23 * 80 + 76)) * 2;

	while (1)
	{
		//char *pT2 = (char *) 0x10000002;
		//*pT2 = 'T';
		*p = *pT;
	}

//	char *p = (char *) 0xb8000;
//	p += ((23 * 80 + 76)) * 2;
//	int i = 33;
//	while (1)
//	{
//		*p = sf;
//		if (++i >= 127)
//		{
//			i = 33;
//		}
//	}
}

/*
 * install_task : 安装多任务
 * return : void
 */
void install_process()
{
	/*
	 * pages:
	 * 	0.pcb
	 * 	1.run
	 * 	2.esp
	 * 	3.esp0
	 * 	4.cr3
	 * 	5.page_dir
	 * 	6.page_tbl [0, 4)M
	 * 	7.page_tbl [4, 8)M
	 * 	8.page_tbl [8, 12)M
	 * 	9.page_tbl [12, 16)M
	 */
	int pages = 16;

	u32 process_id = 1;

	s_pcb *pcb_empty = NULL;
	void* mm_pcb = alloc_page(process_id, pages, 0);
	pcb_empty = (s_pcb *) mm_pcb;
	init_process(mm_pcb, pcb_empty, process_id, NULL);
	process_id++;

	void* mm_pcb_A = alloc_page(process_id, pages, 0);
	pcb_A = (s_pcb *) mm_pcb_A;
	init_process(mm_pcb_A, pcb_A, process_id, &run_A);
	process_id++;

	void* mm_pcb_B = alloc_page(process_id, pages, 0);
	pcb_B = (s_pcb *) mm_pcb_B;
	init_process(mm_pcb_B, pcb_B, process_id, &run_B);
	process_id++;

	addr_to_gdt(GDT_TYPE_TSS, (u32) &pcb_empty->tss, &gdts[4], GDT_G_BYTE, sizeof(s_tss) * 8);
	addr_to_gdt(GDT_TYPE_LDT, (u32) pcb_empty->ldt, &gdts[5], GDT_G_BYTE, sizeof(s_gdt) * 2 * 8);

	//载入tss和ldt
	load_tss(GDT_INDEX_TSS);
	load_ldt(GDT_INDEX_LDT);
}

/*
 * create_task : 创建tts任务
 *  - int type : tts任务类型TASK_TYPE_NOR、TASK_TYPE_SPE
 * return : void
 */
void init_process(void *mm_pcb, s_pcb *pcb, u32 process_id, void *run_addr)
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
	pcb->tss.eip = 0;
	pcb->tss.esp = 0;
	pcb->tss.esp0 = 0;
	pcb->tss.es = USER_DATA_SEL;
	pcb->tss.cs = USER_CODE_SEL;
	pcb->tss.ss = USER_DATA_SEL;
	pcb->tss.ds = USER_DATA_SEL;
	pcb->tss.fs = USER_DATA_SEL;
	pcb->tss.gs = USER_DATA_SEL;
	pcb->tss.ldt = GDT_INDEX_LDT;
	pcb->tss.trace_bitmap = 0x0;

	//设置多任务的gdt描述符
	addr_to_gdt(LDT_TYPE_CS, 0, &(pcb->ldt[0]), GDT_G_KB, 0xfffff);
	addr_to_gdt(LDT_TYPE_DS, 0, &(pcb->ldt[1]), GDT_G_KB, 0xfffff);

	pcb->pid = process_id;
	pcb->tss.eip = (u32) mm_pcb + 0x1000;
	pcb->tss.esp = (u32) mm_pcb + 0x2000;
	pcb->tss.esp0 = (u32) mm_pcb + 0x3000;
	pcb->tss.cr3 = (u32) mm_pcb + 0x4000;
	pcb->swap = NULL;
	mmcopy(run_addr, (void *) (pcb->tss.eip), 0x1000);

	u32 *page_dir = (u32 *) (pcb->tss.cr3);
	u32 *page_tbl = (u32 *) ((u32) mm_pcb + 0x5000);

	u32 address = 0;
	//前16M系统内存
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 1024; j++)
		{
			page_tbl[j] = address | 7;
			address += 0x1000;
		}
		page_dir[i] = ((u32) page_tbl | 7);
		page_tbl += 1024;
	}

	//mm_pcb所在内存
	address = (u32) mm_pcb;

	u32 page_dir_index = (address >> 22) & 0x3ff;
	u32 page_table_index = (address >> 12) & 0x3ff;

	page_tbl = (u32 *) alloc_page(process_id, 1, 0);
	for (int i = 0; i < 1024; i++)
	{
		if (i >= page_table_index && i <= (page_table_index + 16))
		{
			page_tbl[i] = address | 7;
			address += 0x1000;
		}
		else
		{
			page_tbl[i] = 6;
		}
	}
	page_dir[page_dir_index] = ((u32) page_tbl | 7);

	if (page_table_index + 16 >= 1024)
	{
		page_tbl = (u32 *) alloc_page(process_id, 1, 0);
		for (int i = 0; i < 1024; i++)
		{
			if (i < (10 - (1024 - page_table_index)))
			{
				page_tbl[i] = address | 7;
				address += 0x1000;
			}
			else
			{
				page_tbl[i] = 6;
			}
		}
		page_dir[page_dir_index + 1] = ((u32) page_tbl | 7);
	}
}

u32* pcb_page_dir(u32 pid)
{
	s_pcb *pcb = NULL;
	if (pid == 2)
	{
		pcb = pcb_A;
	}
	else if (pid == 3)
	{
		pcb = pcb_B;
	}
	return (u32 *) (pcb->tss.cr3);
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

	//在时钟中断时并没有切换ds和cr3寄存器
	//但是在call tss时cr3会被修改为tss中的cr3
	set_ds(0xf);

	call_tss();
}

#endif
