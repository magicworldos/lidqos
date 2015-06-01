/*
 * start_main.c
 * Copyright (C) Feb 10, 2014 by lidq
 *
 * 启动程序外壳载入:
 *  - 主要用于载入并调用程序的main函数
 *  - 程序运行结束之后再调用中断停止此程序
 */

#include <shell/start_main.h>

extern int main(int argc, char **args);

void start_main()
{
	int argc = 0;
	char **args = NULL;
	main(argc, args);

	u32 sem_addr = 0;

	int params[2];
	//type为1,代表停止进程
	params[0] = 1;
	//请求0x80号中断服务停止此进程
	__asm__ volatile("int	$0x80" :: "a"(params));
}
