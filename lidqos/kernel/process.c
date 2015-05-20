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

u32 process_id = 0;
extern s_list* pcb_cur;

/*
 * install_task : 安装多任务
 * return : void
 */
void install_process()
{
	int pages = sizeof(s_pcb) / MM_PAGE_SIZE;
	if (sizeof(s_pcb) % MM_PAGE_SIZE != 0)
	{
		pages++;
	}
	//空任务
	s_pcb *pcb_empty = alloc_page(process_id, pages, 0, 0);
	init_process(pcb_empty, process_id, NULL, 0, 0);
	//pcb_insert(pcb_empty);
	process_id++;

	//载入TSS
	addr_to_gdt(GDT_TYPE_TSS, (u32) &(pcb_empty->tss), &gdts[4], GDT_G_BYTE, sizeof(s_tss) * 8);
	//载入LDT
	addr_to_gdt(GDT_TYPE_LDT, (u32) (pcb_empty->ldt), &gdts[5], GDT_G_BYTE, sizeof(s_gdt) * 2 * 8);

	//载入tss和ldt
	load_tss(GDT_INDEX_TSS);
	load_ldt(GDT_INDEX_LDT);
}

void install_system()
{
	load_process("/usr/bin/system", "");
}

/*
 * create_task : 创建tts任务
 *  - int type : tts任务类型TASK_TYPE_NOR、TASK_TYPE_SPE
 * return : void
 */
void init_process(s_pcb *pcb, u32 pid, void *run, u32 run_offset, u32 run_size)
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
	addr_to_gdt(LDT_TYPE_CS, 0, &(pcb->ldt[0]), GDT_G_KB, 0xffffffff);
	addr_to_gdt(LDT_TYPE_DS, 0, &(pcb->ldt[1]), GDT_G_KB, 0xffffffff);

	pcb->run = run;
	//设置pcb相关值
	pcb->process_id = pid;
	pcb->tss.eip = (u32) run + run_offset;
	//永远是4G，但为了4K对齐保留了最后一个0x1000
	pcb->tss.esp = 0xfffff000;
	pcb->tss.esp0 = (u32) pcb->stack0 + 0x400;
//	pcb->tss.esp0 = (u32) pcb->stack0 + 0x1000;
	pcb->tss.cr3 = (u32) pcb->page_dir;

	//地址
	u32 address = 0;

	u32 *page_dir = ((u32 *) pcb->page_dir);
	u32 *page_tbl = ((u32 *) pcb->page_tbl);

	/*
	 * 前16M系统内存为已使用
	 * 实际上16M系统内存是不应该让普通程序可写的
	 * 但为了能让普通程序直接操作0xb8000显示缓冲区
	 */
	for (int i = 0; i < 1024; i++)
	{
		for (int j = 0; j < 1024; j++)
		{
			if (i < 4)
			{
				page_tbl[j] = address | 5;
			}
			else
			{
				page_tbl[j] = 6;
			}
			address += 0x1000;
		}
		page_dir[i] = (u32) page_tbl | 7;
		page_tbl += 1024;
	}

	int pages = sizeof(s_pcb) / MM_PAGE_SIZE;
	if (sizeof(s_pcb) % MM_PAGE_SIZE != 0)
	{
		pages++;
	}
	init_process_page((u32) pcb, pages, pcb->page_dir);
}

void init_process_page(u32 address, u32 pages, u32 *page_dir)
{
	//mm_pcb所在内存页目录索引
	u32 page_dir_index = (address >> 22) & 0x3ff;
	//mm_pcb所在内存页表索引
	u32 page_table_index = (address >> 12) & 0x3ff;
	address &= 0xFFC00000;
	u32 *page_tbl = (u32 *) (page_dir[page_dir_index] & 0xfffff000);

	//mm_pcb所在内存页表
	for (int i = 0; i < 1024; i++)
	{
		//设置mm_pcb所在内存的页表
		if (i >= page_table_index && i <= (page_table_index + 16))
		{
			page_tbl[i] = address | 5;
		}
		address += 0x1000;
	}
	//设置mm_pcb所在内存的页目录
	page_dir[page_dir_index] = (u32) page_tbl | 7;

	//设置pages个页面剩余页
	if (page_table_index + pages >= 1024)
	{
		page_dir_index++;
		page_tbl += 1024;
		for (int i = 0; i < 1024; i++)
		{
			if (i < (pages - (1024 - page_table_index)))
			{
				page_tbl[i] = address | 5;

			}
			address += 0x1000;
		}
		page_dir[page_dir_index] = (u32) page_tbl | 7;
	}
}

s_pcb* load_process(char *file_name, char *params)
{
	//从文件系统读入程序
	s_file *fp = f_open(file_name, FS_MODE_READ, 0, 0);
	int pages = fp->fs.size / MM_PAGE_SIZE;
	if (fp->fs.size % MM_PAGE_SIZE != 0)
	{
		pages++;
	}
	void *run = alloc_page(process_id, pages, 0, 0);
	f_read(fp, fp->fs.size, (u8 *) run);
	u32 run_size = fp->fs.size;
	//关闭文件
	f_close(fp);
	//对elf可执行程序进行重定位
	relocation_elf(run);

	//取得程序运行开始位置shell_run + phdr.p_offset
	Elf32_Ehdr ehdr;
	mmcopy_with(run, (void *) &ehdr, 0, sizeof(Elf32_Ehdr));
	Elf32_Phdr phdr;
	mmcopy_with(run, (void *) &phdr, ehdr.e_phoff, sizeof(Elf32_Phdr));

	pages = sizeof(s_pcb) / MM_PAGE_SIZE;
	if (sizeof(s_pcb) % MM_PAGE_SIZE != 0)
	{
		pages++;
	}
	//任务
	s_pcb *pcb = alloc_page(process_id, pages, 0, 0);
//	pcb->stack0 = alloc_page(process_id, 1, 0, 0);
	pcb->page_dir = alloc_page(process_id, 1, 0, 0);
	pcb->page_tbl = alloc_page(process_id, 0x400, 0, 0);
	init_process(pcb, process_id, run, phdr.p_offset, run_size);
	pcb_insert(pcb);
	process_id++;
	alloc_page(process_id, 1, 0, 0);
	return pcb;
}

/*
 * relocation_elf :  elf可执行文件重定位
 *  - void *addr : 可执行程序地址
 * return : void
 */
void relocation_elf(void *addr)
{
	//elf文件头
	Elf32_Ehdr ehdr;
	mmcopy_with((char *) addr, (char *) &ehdr, 0, sizeof(Elf32_Ehdr));

	//程序头
	Elf32_Phdr phdr;
	mmcopy_with((char *) addr, (char *) &phdr, ehdr.e_phoff, sizeof(Elf32_Phdr));

	Elf32_Shdr shdrs[ehdr.e_shnum];
	Elf32_Shdr shdr_rel;
	//循环程序头
	for (int i = 0; i < ehdr.e_shnum; ++i)
	{
		mmcopy_with((char *) addr, (char *) &shdrs[i], ehdr.e_shoff + (i * sizeof(Elf32_Shdr)), sizeof(Elf32_Shdr));
		//如果需要重定位
		if (shdrs[i].sh_type == 9)
		{
			//加入需要重定位的列表
			mmcopy_with((char *) addr, (char *) &shdr_rel, ehdr.e_shoff + (i * sizeof(Elf32_Shdr)), sizeof(Elf32_Shdr));
		}
	}
	u32 rels_count = shdr_rel.sh_size / sizeof(Elf32_Rel);
	Elf32_Rel rels[rels_count];

	//计算重定位后的物理地址
	u32 p_offset = (u32) addr + phdr.p_offset;

	//循环重定位列表
	for (int i = 0; i < rels_count; ++i)
	{
		//取得重定位项
		mmcopy_with((char *) addr, (char *) &rels[i], shdr_rel.sh_offset + (i * sizeof(Elf32_Rel)), sizeof(Elf32_Rel));
		if (rels[i].r_info == 1)
		{
			//重定位
			u32 *p = (u32*) (p_offset + rels[i].r_offset);
			*p += p_offset;
		}
	}
}

u32* pcb_page_dir(u32 pid)
{
	s_pcb* pcb = (s_pcb*) pcb_cur->node;
	return (u32 *) (pcb->tss.cr3);
}

/*
 * get_current_task_id : 取得当前运行的任务ID
 * return : int任务ID
 */
u32 get_current_process_id()
{
	return get_current_process()->process_id;
}

/*
 * get_current_task : 取得当前运行的任务指针
 * return : s_task*任务指针
 */
s_pcb* get_current_process()
{
	return (s_pcb*) pcb_cur->node;
}

#endif
