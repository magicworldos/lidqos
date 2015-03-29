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
	//横坐标超出80换行
	if (x > 80)
	{
		x = 0;
		y++;
	}
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

/***
 * 根据一个字符的ascii显示到指定位置
 * u16 x: 横坐标
 * u16 y: 纵坐标
 * char ch: 要显示的字符
 */
void putascii(u16 x, u16 y, char ch)
{
	//定义显存地址
	char *video_addr = (char *) 0xb8000;

	//写入显存
	u32 where = (y * 80 + x) * 2;
	//显示字符的实际物理地址
	u8 *p = (u8 *) (video_addr) + where;
	//字符的ascii码
	*p = ch;
	//颜色
	*(p + 1) = 0x07;
}

/***
 * 显示一个字符到当前光标位置
 * char ch: 要显示的字符
 */
void putchar(char ch)
{
	//取得当前光标线性位置
	u16 cursor_pos = get_cursor();
	//计算横纵坐标
	u16 x = cursor_pos % 80;
	u16 y = cursor_pos / 80;

	//如果是换行符\n
	if (ch == 0xa)
	{
		//换行
		x = 0;
		y++;
		set_cursor(x, y);
	}
	//如果是制表符\t
	else if (ch == 0x9)
	{
		//显示空格
		ch = 0x20;
		//显示8个空格
		for (int i = 0; i < 8; i++)
		{
			putascii(x, y, ch);
			x++;
			set_cursor(x, y);
		}
	}
	//显示普通字符
	else
	{
		putascii(x, y, ch);
		x++;
		set_cursor(x, y);
	}
}

int printf(char *fmt, ...)
{
	return 0;
}

#endif

