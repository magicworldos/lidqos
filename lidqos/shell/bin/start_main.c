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
	//为main函数传入参数个数
	int argc = 0;
	//为main函数传入参数表
	char **args = NULL;

	//运行参数
	char *pars = malloc(CMD_ARGS_SIZE);
	int params[2];
	params[0] = 5;
	params[1] = (int) pars;
	//取得pcb的运行参数内容
	__asm__ volatile("int	$0x80" :: "a"(params));

	//计算参数个数
	argc = str_split_count(pars, ' ');
	//为二级字符串指针申请内存
	args = malloc(argc * sizeof(char *));
	//为每个字符串申请内存
	for (int i = 0; i < argc; ++i)
	{
		//申请内存
		args[i] = malloc(CMD_ARGS_EVERY_SIZE);
	}
	//将字符串参数分割为字符串数组
	str_split(pars, ' ', args);
	//调用main函数，并传入参数
	main(argc, args);
	//释放main函数参数所占用的内存
	for (int i = 0; i < argc; ++i)
	{
		free(args[i]);
	}
	//释放内存
	free(args);
	free(pars);

	//type为1,代表停止进程
	params[0] = 1;
	//请求0x80号中断服务停止此进程
	__asm__ volatile("int	$0x80" :: "a"(params));
}
