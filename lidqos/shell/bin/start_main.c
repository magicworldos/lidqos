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

//外部被调用的函数
extern int main(int argc, char *args[]);

/*
 * start_main : 启动函数
 * return : void
 */
void start_main()
{
	//正式调用main函数
	main(1, NULL);

	for (;;)
	{
	}
}

#endif
