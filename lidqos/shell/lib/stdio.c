/*
 * printf.c
 * Copyright (C) Feb 10, 2014 by lidq
 *
 * 标准输出:
 *  - 用于向标准设备显示字符内容
 */

#ifndef _KPRINTF_C_
#define _KPRINTF_C_

#include <shell/stdio.h>

/*
 * putch : 显示一个字符
 *  - char ch : 待显示字符
 * return : void
 */
void putch(char ch)
{
	int params[2];
	params[0] = 0;
	params[1] = ch;
	__asm__ volatile ("int $0x82" :: "a"(params));
}

/*
 * putchar : 显示一个字符，只显示可显字符
 *  - char ch : 待显示字符
 * return : void
 */
void putchar(char ch)
{
	if ((ch >= 0x20 && ch <= 0x7e) || ch == '\n' || ch == '\t')
	{
		putch(ch);
	}
}

/*
 * puts : 显示字符串
 *  - char *str : 字符串
 * return : void
 */
int puts(char *str)
{
	int count = 0;
	int params[2];
	params[0] = 1;
	params[1] = (int) str;
	params[2] = (int) &count;
	__asm__ volatile ("int $0x82" :: "a"(params));
	return count;
}

/*
 * number_to_str : 将整数转为字符串
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

	while (number > 0)
	{
		rem = number % hex;
		temp[i++] = num[rem];
		number = number / hex;
	}
	if (sign == '-')
	{
		temp[i++] = sign;
	}
	length = i;

	for (i = length - 1; i >= 0; i--)
	{
		*buff++ = temp[i];
	}
}

/*
 * printf : 标准设备显示函数
 *  - char *fmt : 显示格式
 *  - ... : 动态参数
 * return : int显示字符数
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

char getchar()
{
	char ch = 0;
	//取得全局按键信号量
	s_sem *sem = get_global_sem(0);
	while (ch == 0)
	{
		sem_wait(sem);
		int params[2];
		params[0] = 0x10;
		params[1] = (int)&ch;
		__asm__ volatile ("int $0x82" :: "a"(params));
		sem_post(sem);
	}

	return ch;
}

void backspace()
{
	int params[2];
	params[0] = 0x11;
	__asm__ volatile ("int $0x82" :: "a"(params));
}

void gets(char *str)
{
	char ch, *p = str;
	while ((ch = getchar()) != '\n')
	{
		if (ch == 0x8)
		{
			if (p > str)
			{
				int back_num = 1;
				if (*(p - 1) == '\t')
				{
					back_num = 4;
				}
				for (int i = 0; i < back_num; ++i)
				{
					p--;
					backspace();
				}
			}
		}
		else
		{
			*p = ch;
			p++;
			putchar(ch);
		}
	}
	*p = '\0';
	putchar('\n');
}

/*
 * get_int : 读入一个整数
 * return : int读入整数
 */
int get_int()
{
	char str[0x800];
	char ch, *p = str;
	char sign = 0;
	do
	{
		ch = getchar();
		if (ch == 0x8)
		{
			if (p > str)
			{
				p--;
				backspace();
			}
		}
		else if (sign == 0 && (ch == '+' || ch == '-'))
		{
			sign = ch;
			putchar(ch);
		}
		else if (ch == '\n')
		{
			putchar(ch);
			break;
		}
		else // if (ch >= '0' && ch <= '9')
		{
			*p = ch;
			p++;
			putchar(ch);
		}
	}
	while (1);
	*p = '\0';
	p = str;

	int num = 0;
	for (int i = 0; p[i] != '\0'; i++)
	{
		if (i == 0)
		{
			num = (int) p[i] - 48;
		}
		else
		{
			num *= 10;
			num += (int) p[i] - 48;
		}
	}
	if (sign == '-')
	{
		return -num;
	}
	return num;
}

/*
 * scanf : 按指定格式读入
 *  - char *fmt : 格式字符串
 *  - ... : 动态参数
 * return : void
 */
void scanf(char *fmt, ...)
{
	va_list args;
	va_init(args, fmt);
	while (*fmt != '\0')
	{
		if (*fmt == '%')
		{
			if ('c' == *(fmt + 1))
			{
				char *p = (char *) va_arg(args, u32);
				*p = getchar();
				putchar(*p);
				fmt += 2;
			}
			else if ('s' == *(fmt + 1))
			{
				char *str = (char *) va_arg(args, u32);
				gets(str);
				fmt += 2;
			}
			else if ('d' == *(fmt + 1))
			{
				int *p = (int *) va_arg(args, u32);
				*p = get_int();
				fmt += 2;
			}
			else
			{
				break;
			}
		}
		else
		{
			fmt++;
		}
	}
}

#endif
