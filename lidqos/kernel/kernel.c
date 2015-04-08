/*
 * kernel.c
 *
 *  Created on: Mar 26, 2015
 *      Author: lidq
 */

#include <kernel/kernel.h>

//全局字符串指针变量
char *str = "Hello World!";

//内核启动程序入口
int start_kernel(int argc, char **args)
{
	//安装内存申请模块
	install_alloc();

	char *p = alloc_page(1);
	printf("%x\n", p);

	char *p1 = alloc_mm(4);
	printf("%x\n", p1);

	char *p2 = alloc_mm(4);
	printf("%x\n", p2);

	free_mm(p2, 4);
	free_mm(p1, 4);
	free_page(p, 1);

	p = alloc_page(1);
	printf("%x\n", p);

	p1 = alloc_mm(4);
	printf("%x\n", p1);

	p2 = alloc_mm(4);
	printf("%x\n", p2);

	free_mm(p2, 4);
	free_mm(p1, 4);
	free_page(p, 1);

	//永无休止的循环
	for (;;)
	{
	}
	return 0;
}
