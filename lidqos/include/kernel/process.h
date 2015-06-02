/*
 * task.h
 * Copyright (C) Feb 10, 2014 by lidq
 *
 * 多任务处理头文件
 */

#ifndef _PROCESS_H_
#define _PROCESS_H_

#include <kernel/typedef.h>
#include <kernel/alloc.h>
#include <kernel/mm.h>
#include <kernel/io.h>
#include <kernel/elf.h>
#include <kernel/page.h>
#include <kernel/sys_var.h>
#include <kernel/list.h>
#include <kernel/sche.h>
#include <kernel/malloc.h>

/*
 * 计算pcb所占用的页面数
 */
int pages_of_pcb();

/*
 * 安装多任务
 */
void install_process();

/*
 * 安装system程序
 */
void install_system();

/*
 * 载入文件系统中的可执行程序
 */
s_pcb* load_process(char *file_name, char *params, int *status);

/*
 * 创建tts任务
 *  - int type : tts任务类型TASK_TYPE_NOR、TASK_TYPE_SPE
 */
void init_process(s_pcb *pcb, u32 pid, void *run, u32 run_offset, u32 run_size);

/*
 * 将address地址加入到页表中
 */
void init_process_page(u32 address, u32 pages, u32 *page_dir);

int check_elf_file(Elf32_Ehdr *ehdr);

/*
 * elf可执行文件重定位
 *  - void *addr : 可执行程序地址
 * return : u32 程序入口地址
 */
int relocation_elf(void *addr, u32 *entry_point);

/*
 * 对.text段或.data段进程重定位
 */
void relocation_elf_text_data(void *addr, Elf32_Shdr sh_rel, u32 rel_num, u32 sh_offset, Elf32_Sym *syms, u32 syms_num, Elf32_Shdr *shdrs);

/*
 * 计算符号重定位地址
 */
u32 relocation_elf_sym(u32 sym, Elf32_Sym *syms, u32 syms_num, Elf32_Shdr *shdrs);

/*
 * 取得当前进程的cr3
 */
u32* pcb_page_dir(u32 pid);

/*
 * 取得当前运行的任务ID
 * return : int任务ID
 */
u32 get_current_process_id();

/*
 * 取得当前运行的任务指针
 * return : s_task*任务指针
 */
s_pcb* get_current_process();

void init_pthread(s_pcb *pcb, u32 pid);

void create_pthread(s_pcb *parent_pcb, s_pthread *p, void *run, void *args);

void* pcb_malloc(s_pcb *pcb, int size);

void pcb_free(s_pcb *pcb, void *addr);

#endif
