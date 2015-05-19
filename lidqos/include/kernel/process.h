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

/*
 * install_task : 安装多任务
 * return : void
 */
void install_process();

void install_system();

/*
 * create_task : 创建tts任务
 *  - int type : tts任务类型TASK_TYPE_NOR、TASK_TYPE_SPE
 * return : void
 */
void init_process(s_pcb *pcb, u32 process_id, void *run, u32 run_offset, u32 run_size);

void init_process_page(u32 address,u32 pages, u32 *page_dir);

s_pcb* load_process(char *file_name, char *params);

/*
 * relocation_elf :  elf可执行文件重定位
 *  - void *addr : 可执行程序地址
 * return : void
 */
void relocation_elf(void *addr);

u32* pcb_page_dir(u32 pid);

/*
 * get_current_task_id : 取得当前运行的任务ID
 * return : int任务ID
 */
u32 get_current_process_id();
/*
 * get_current_task : 取得当前运行的任务指针
 * return : s_task*任务指针
 */
s_pcb* get_current_process();

#endif
