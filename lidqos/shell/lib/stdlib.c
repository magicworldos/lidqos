/*
 * printf.c
 * Copyright (C) Feb 10, 2014 by lidq
 *
 * 标准输出:
 *  - 用于向标准设备显示字符内容
 */

#ifndef _KPRINTF_C_
#define _KPRINTF_C_

#include <shell/stdlib.h>

//int rand()
//{
//	int ret = 0;
//	int params[2];
//	params[0] = 0;
//	params[1] = (int) &ret;
//	__asm__ volatile("int	$0x83" :: "a"(params));
//	return ret;
//}
//
//void srand(u32 seed)
//{
//	int params[2];
//	params[0] = 1;
//	params[1] = (int) seed;
//	__asm__ volatile("int	$0x83" :: "a"(params));
//}

int random(int min, int max)
{
	int ret = 0;
	int params[4];
	params[0] = 2;
	params[1] = min;
	params[2] = max;
	params[3] = (int) &ret;
	__asm__ volatile("int	$0x83" :: "a"(params));
	return ret;
}

void* malloc(int size)
{
	void *ret = NULL;
	int params[3];
	params[0] = 0x10;
	params[1] = size;
	params[2] = (int) &ret;
	__asm__ volatile("int	$0x83" :: "a"(params));
	return ret;
}

void free(void *addr)
{
	int params[2];
	params[0] = 0x11;
	params[1] = (int) addr;
	__asm__ volatile("int	$0x83" :: "a"(params));
}

#endif
