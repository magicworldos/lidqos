/*
 * printf.c
 * Copyright (C) Feb 10, 2014 by lidq
 *
 * 标准输出:
 *  - 用于向标准设备显示字符内容
 */

#ifndef _KPRINTF_C_
#define _KPRINTF_C_

#include <kernel/printf.h>

/***
 * 设置光标位置
 * u16 x: 光标的横坐标
 * u16 y: 光标的纵坐标
 */
void set_cursor(u16 x, u16 y)
{
	//计算光标的线性位置
	u16 cursor_pos = y * 80 + x;
	//告诉地址寄存器要接下来要使用14号寄存器
	outb_p(14, 0x03d4);
	//向光标位置高位寄存器写入值
	outb_p((cursor_pos >> 8) & 0xff, 0x03d5);
	//告诉地址寄存器要接下来要使用15号寄存器
	outb_p(15, 0x03d4);
	//向光标位置高位寄存器写入值
	outb_p(cursor_pos & 0xff, 0x03d5);
}

/***
 * 取得光标位置
 * return: 光标的线性位置
 */
u16 get_cursor()
{
	//告诉地址寄存器要接下来要使用14号寄存器
	outb_p(14, 0x03d4);
	//从光标位置高位寄存器读取值
	u8 cursor_pos_h = inb_p(0x03d5);
	//告诉地址寄存器要接下来要使用15号寄存器
	outb_p(15, 0x03d4);
	//从光标位置高位寄存器读取值
	u8 cursor_pos_l = inb_p(0x03d5);
	//返回光标位置
	return (cursor_pos_h << 8) | cursor_pos_l;
}

void putchar(char ch)
{
}

int printf(char *fmt, ...)
{
	return 0;
}

#endif
