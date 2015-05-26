/*
 * printf.c
 * Copyright (C) Feb 10, 2014 by lidq
 *
 * 标准输出:
 *  - 用于向标准设备显示字符内容
 */

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

	//纵坐标超出25滚屏
	if (y >= 25)
	{
		x = 0;
		y = 25 - 1;
		scroll_up(1);
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

/*
 * scroll_up : 向上滚屏
 *  - int int row : 行数
 * return : void
 */
void scroll_up(int row)
{
	u16 *p = (u16 *) 0xb8000;
	for (int i = 0; i < 80; ++i)
	{
		for (int j = 0; j < 25; ++j)
		{
			*(p + j * 80 + i) = *(p + (j + 1) * 80 + i);
		}
	}
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

void backspace()
{
	//取得当前光标线性位置
	u16 cursor_pos = get_cursor();
	if (cursor_pos > 0)
	{
		int x, y;
		get_cursor(&x, &y);
		putascii(x, y, ' ');
		cursor_pos--;
		set_cursor(x, y);
	}
}

/*
 * number_to_str : 将整数转为字符串
 *  - int tty_id : tty编号
 *  - char *buff : 数据地址
 *  - int number : 整数
 *  - int hex : 10进制或16进制
 * return : void
 */
void number_to_str(char *buff, int number, int hex)
{
	char temp[0x800];
	char num[0x20] = "0123456789ABCDEFG";

	int i = 0;
	int length = 0;
	int rem;
	char sign = '+';

	//反向加入temp
	temp[i++] = '\0';
	if (number < 0)
	{
		sign = '-';
		number = 0 - number;
	}
	else if (number == 0)
	{
		temp[i++] = '0';
	}

	//将数字转为字符串
	while (number > 0)
	{
		rem = number % hex;
		temp[i++] = num[rem];
		number = number / hex;
	}
	//处理符号
	if (sign == '-')
	{
		temp[i++] = sign;
	}
	length = i;

	//返向拷贝到buff缓冲区
	for (i = length - 1; i >= 0; i--)
	{
		*buff++ = temp[i];
	}
}

/*
 * puts : 显示字符串
 *  - int tty_id : tty编号
 *  - char *str : 字符串
 * return : void
 */
int puts(char *str)
{
	int count = 0;
	while (*str != '\0')
	{
		putchar(*str++);
		count++;
	}
	return count;
}

/***
 * 标准输出函数
 * char *fmt: 格式化字符串
 * 动态参数
 * return: 显示字符个数
 */
int printf(char *fmt, ...)
{
	//显示数字缓冲区
	char buff[0x800];
	//显示字符串指针
	char *str;
	//显示字符变量
	char ch;
	//显示字符总数
	int count = 0;

	//动态参数
	va_list args;
	//初始化动态参数
	va_init(args, fmt);

	//读到\0为结束
	while (*fmt != '\0')
	{
		//格式化标记%
		if (*fmt == '%')
		{
			//显示一个字符
			if ('c' == *(fmt + 1))
			{
				ch = va_arg(args, char);
				putchar(ch);
				count++;
				fmt += 2;
			}
			//显示字符串
			else if ('s' == *(fmt + 1))
			{
				str = va_arg(args, char*);
				count += puts(str);
				fmt += 2;
			}
			//显示整数
			else if ('d' == *(fmt + 1))
			{
				number_to_str(buff, va_arg(args, int), 10);
				count += puts(buff);
				fmt += 2;
			}
			//显示无符号16进制整数
			else if ('x' == *(fmt + 1))
			{
				number_to_str(buff, va_arg(args, u32), 16);
				count += puts(buff);
				fmt += 2;
			}
		}
		//显示普通字符
		else
		{
			putchar(*fmt++);
			count++;
		}
	}
	return count;
}
