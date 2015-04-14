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

//可显字符
extern u8 keys[0x53][2];
//shift键按下状态
u8 kb_key_shift = 0;
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

/*
 * int_keyboard : 键盘中断
 * return : void
 */
void int_keyboard()
{
	//取得扫描码
	u8 scan_code = inb_p(0x60);
	//取得按下、抬起状态
	u8 status = scan_code >> 7;
	//扫描码的索引
	u8 key_ind = scan_code & 0x7f;
	//shift按下
	if ((key_ind == KEY_LEFT_SHIFT || key_ind == KEY_RIGHT_SHIFT) && status == 0)
	{
		kb_key_shift = 0x1;
	}
	//shift抬起
	else if ((key_ind == KEY_LEFT_SHIFT || key_ind == KEY_RIGHT_SHIFT) && status == 1)
	{
		kb_key_shift = 0x0;
	}
	else if (status == 0)
	{
		//显示字符
		putchar(keys[key_ind - 1][kb_key_shift]);
	}
	//清除键盘状态可以接受新按键
	outb_p(scan_code & 0x7f, 0x61);
	//通知PIC1可以接受新中断
	outb_p(0x20, 0x20);
}

#endif
