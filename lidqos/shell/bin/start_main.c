/*
 * start_main.c
 * Copyright (C) Feb 10, 2014 by lidq
 *
 * 启动程序外壳载入:
 *  - 主要用于载入并调用程序的main函数
 *  - 程序运行结束之后再调用中断停止此程序
 */

#ifndef _START_MAIN_C_
#define _START_MAIN_C_

#include <shell/start_main.h>

extern int main(int argc, char *args[]);

unsigned int gg1 = 0x1111;
unsigned int gg2 = 0x2222;
unsigned int gg3 = 0x3333;
unsigned int gg4 = 0x4444;

char *stra1 = "aaaa";
char *stra2 = "bbbb";

void testff(char *p);

void start_main()
{
	gg1 = 0x5555;
	gg2 = 0x6666;
	gg3 = 0x7777;
	gg4 = 0x8888;

	testff(stra1);
	testff(stra2);

	main(1, 0);

	for (;;)
	{
	}
}

#endif
