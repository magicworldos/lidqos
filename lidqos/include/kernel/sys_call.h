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
#include <kernel/printf.h>

void int_div_error();

void int_timer();

void int_keyboard();

#endif
