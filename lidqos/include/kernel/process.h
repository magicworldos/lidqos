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
#include <kernel/page.h>

/*
 * map_process中存放process的ID为u32类型，
 * 所以在以后的process数据结构中process_id也要是u32类型
 */

void run_A();

void run_B();

void install_process();

void init_process(void *mm_pcb, s_pcb *pcb, u32 process_id, void *run_addr);

u32* pcb_page_dir(u32 pid);

void schedule();

#endif
