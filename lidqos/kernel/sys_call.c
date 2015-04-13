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

/*
 * int_timer : 时钟中断
 * return : void
 */
void int_timer()
{
	//一直循环
	for (int i = 0;; ++i)
	{
		//一直在第1行的行首处显示
		set_cursor(0, 1);
		//显示i的值
		printf("%d", i);
	}

	//通知PIC可以接受新中断
	outb_p(0x20, 0x20);
}

#endif
