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
	//显存地址
	char *p = (char *) 0xb8000;
	//显示str的内容到显示器上
	for (int i = 0; str[i] != '\0'; i++)
	{
		//显示字符
		p[i * 2] = str[i];
		//取得光标位置
		u16 cursor_pos = get_cursor();
		//下一个光标位置
		cursor_pos++;
		//设置光标到新位置
		set_cursor(cursor_pos % 80, cursor_pos / 80);
	}

	//永无休止的循环
	for (;;)
	{
	}
	return 0;
}
