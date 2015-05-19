/*
 * sche.h
 * Copyright (C) Feb 10, 2014 by lidq
 *
 * 调度程序头文件
 */

#ifndef _SCHE_H_
#define _SCHE_H_

#include <kernel/typedef.h>
#include <kernel/process.h>
#include <kernel/mm.h>
#include <kernel/alloc.h>
#include <kernel/string.h>
#include <kernel/list.h>
#include <kernel/sys_var.h>
#include <kernel/config.h>
#include <kernel/io.h>
#include <kernel/fs.h>

void schedule();

void pcb_insert(s_pcb *pcb);

void set_process_ready(s_pcb *pcb);

void set_process_wait(s_pcb *pcb);

void set_process_stop(s_list *p_list);

s_list* get_process(int process_id);

void free_process();

void kill_process(int process_id);

#endif /* SCHE_H_ */
