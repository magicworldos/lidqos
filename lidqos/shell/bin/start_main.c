/*
 * start_main.c
 * Copyright (C) Feb 10, 2014 by lidq
 *
 * 启动程序外壳载入:
 *  - 主要用于载入并调用程序的main函数
 *  - 程序运行结束之后再调用中断停止此程序
 */

#include <shell/start_main.h>

extern int main(int argc, char *args[]);

void start_main()
{
	main(1, NULL);

	for (;;)
	{
	}
}
