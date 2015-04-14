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

//时钟中断时显示字符起始ascii
int i = 32;

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
	char *p = (char *) 0xb8000;
	p += ((23 * 80 + 77)) * 2;
	*p = i;
	if (++i >= 127)
	{
		i = 32;
	}
	//通知PIC可以接受新中断
	outb_p(0x20, 0x20);
}

#endif
