/*
 * printf.c
 * Copyright (C) Feb 10, 2014 by lidq
 *
 * 标准输出:
 *  - 用于向标准设备显示字符内容
 */

#include <kernel/printf.h>

extern s_tty *tty_cur;
extern s_sys_var *sys_var;

void draw_cursor(int tty_id, int x, int y)
{
	//绘制光标字符到光标位置
	putascii(tty_id, x, y, CURSOR_CHAR);
}

void clear_cursor(int tty_id, int x, int y)
{
	//绘制空白字符到光标位置
	putascii(tty_id, x, y, ' ');
}

/***
 * 设置光标位置
 * u16 x: 光标的横坐标
 * u16 y: 光标的纵坐标
 */
void set_cursor(int tty_id, u32 x, u32 y)
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
		scroll_up(tty_id, 1);
	}

	//计算光标的线性位置
	u32 cursor_pos = y * 80 + x;

	sys_var->ttys[tty_id].cursor_pos = cursor_pos;

	draw_cursor(tty_id, x, y);
}

/***
 * 取得光标位置
 * return: 光标的线性位置
 */
u32 get_cursor(int tty_id)
{
	return sys_var->ttys[tty_id].cursor_pos;
}

/*
 * scroll_up : 向上滚屏
 *  - int int row : 行数
 * return : void
 */
void scroll_up(int tty_id, int row)
{
	u16 *p = NULL;
	if (tty_id == tty_cur->tty_id)
	{
		p = (u16 *) 0xb8000;
	}
	else
	{
		p = (u16 *) sys_var->ttys[tty_id].mm_addr;
	}

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
void putascii(int tty_id, u32 x, u32 y, char ch)
{
	//定义显存地址
	char *video_addr = NULL;

	if (tty_id == tty_cur->tty_id)
	{
		video_addr = (char *) 0xb8000;
	}
	else
	{
		video_addr = sys_var->ttys[tty_id].mm_addr;
	}

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
void putchar(int tty_id, char ch)
{
	//取得当前光标线性位置
	u32 cursor_pos = get_cursor(tty_id);
	//计算横纵坐标
	u32 x = cursor_pos % 80;
	u32 y = cursor_pos / 80;

	clear_cursor(tty_id, x, y);

	//如果是换行符\n
	if (ch == 0xa)
	{
		//换行
		x = 0;
		y++;
		//set_cursor(tty_id, x, y);
	}
	//如果是制表符\t
	else if (ch == 0x9)
	{
		//显示空格
		ch = 0x20;
		//显示8个空格
		for (int i = 0; i < 4; i++)
		{
			putascii(tty_id, x, y, ch);
			x++;
			//set_cursor(tty_id, x, y);
		}
	}
	//显示普通字符
	else
	{
		putascii(tty_id, x, y, ch);
		x++;
		//set_cursor(tty_id, x, y);
	}
	set_cursor(tty_id, x, y);
}

void backspace(int tty_id)
{
	//取得当前光标线性位置
	u32 cursor_pos = get_cursor(tty_id);
	u32 x = cursor_pos % 80;
	u32 y = cursor_pos / 80;
	clear_cursor(tty_id, x, y);
	cursor_pos--;
	if (cursor_pos > 0)
	{
		x = cursor_pos % 80;
		y = cursor_pos / 80;
		set_cursor(tty_id, x, y);
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
int puts(int tty_id, char *str)
{
	int count = 0;
	while (*str != '\0')
	{
		putchar(tty_id, *str++);
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
int printf(int tty_id, char *fmt, ...)
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
				putchar(tty_id, ch);
				count++;
				fmt += 2;
			}
			//显示字符串
			else if ('s' == *(fmt + 1))
			{
				str = va_arg(args, char*);
				count += puts(tty_id, str);
				fmt += 2;
			}
			//显示整数
			else if ('d' == *(fmt + 1))
			{
				number_to_str(buff, va_arg(args, int), 10);
				count += puts(tty_id, buff);
				fmt += 2;
			}
			//显示无符号16进制整数
			else if ('x' == *(fmt + 1))
			{
				u32 num = va_arg(args, u32);
				u32 nl = num & 0xffff;
				u32 nh = (num >> 16) & 0xffff;
				count += puts(tty_id, "0x");
				if (nh == 0)
				{
					number_to_str(buff, nl, 16);
					count += puts(tty_id, buff);
				}
				else
				{
					number_to_str(buff, nh, 16);
					count += puts(tty_id, buff);

					number_to_str(buff, nl, 16);
					int zero = 4 - str_len(buff);
					for (int i = 0; i < zero; i++)
					{
						putchar(tty_id, '0');
					}
					count += puts(tty_id, buff);
				}
				fmt += 2;
			}
		}
		//显示普通字符
		else
		{
			putchar(tty_id, *fmt++);
			count++;
		}
	}
	return count;
}
