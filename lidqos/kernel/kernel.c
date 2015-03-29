/*
 * kernel.c
 *
 *  Created on: Mar 26, 2015
 *      Author: lidq
 */

#include <kernel/kernel.h>
#include <kernel/printf.h>

//全局字符串指针变量
char *str = "Hello World!";

//内核启动程序入口
int start_kernel(int argc, char **args)
{
	//显示str的内容到显示器上
	for (int i = 0; str[i] != '\0'; i++)
	{
		//显示一个字符到光标所在位置，光标移动一个位置
		putchar(str[i]);
	}

	printf("", 'A', 'B', 'C', 'D', 'E');

	//永无休止的循环
	for (;;)
	{
	}
	return 0;
}
