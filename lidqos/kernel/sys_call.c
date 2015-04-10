/*
 * sys_call.c
 * Copyright (C) Feb 10, 2014 by lidq
 *
 * 系统中断程序
 *  - 处理异常中断程序
 *  - 处理系统中断
 */

#ifndef _SYS_CALL_C_
#define _SYS_CALL_C_

#include <kernel/sys_call.h>

void int_div_error()
{
	printf("div error.\n");
}

#endif
