/*
 * sys_call.h
 * Copyright (C) Feb 10, 2014 by lidq
 *
 * 系统中断程序头文件
 */

#ifndef _SYS_CALL_H_
#define _SYS_CALL_H_

#include <kernel/typedef.h>
#include <kernel/io.h>
#include <kernel/key.h>
#include <kernel/process.h>
#include <kernel/page.h>
#include <kernel/printf.h>
#include <kernel/sche.h>
#include <kernel/rand.h>
#include <kernel/malloc.h>

void int_div_error();

void int_invalid_opcode();

void int_protection_error();

void int_timer();

void int_keyboard();

void* addr_parse(u32 cr3, void *addr);

void sys_process(int *params);

void sys_stdio(int *params);

void sys_stdlib(int *params);

void sys_hd_rw(int *params);

void sys_pts(int *params);

void sys_file(int *params);

void sys_fs(int *params);

#endif
