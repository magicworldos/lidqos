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

/*
 * 进程调度，目前只使用平均时间片轮转的算法
 */
void schedule();

/*
 * 将进程加入到链表中
 */
void pcb_insert(s_pcb *pcb);

void list_sleep_change();

void list_block_change();

void pcb_sleep(s_pcb *pcb, int ms);

int pcb_sem_P(s_pcb *pcb, s_sem *sem);

int pcb_sem_V(s_pcb *pcb, s_sem *sem);

#endif /* SCHE_H_ */
