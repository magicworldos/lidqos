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
	//显示Hello World!
	printf("%s\n", str);

	u32 no = 0x12051204;
	int age = 33;
	char ch = 'B';
	char *msg = "Lidq school number is %x.\nThis year he is %d years old.\nHe got an %c on his test points.\n";
	printf(msg, no, age, ch);

	//永无休止的循环
	for (;;)
	{
	}
	return 0;
}
