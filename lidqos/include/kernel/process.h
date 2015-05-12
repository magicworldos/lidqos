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

void run_A();

void run_B();

void install_process();

void init_process(s_pcb *pcb);

void schedule();

#endif
