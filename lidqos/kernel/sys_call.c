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

/*
 * int_div_error : 除零错
 * return : void
 */
void int_div_error()
{
	printf("Div error.\n");
	hlt();
}

#endif
