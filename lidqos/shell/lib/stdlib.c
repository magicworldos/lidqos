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

void* realloc(void *addr, int size)
{
	void *new_addr = malloc(size);
	if (new_addr == NULL)
	{
		return addr;
	}
	memcpy(addr, new_addr, size);
	free(addr);
	return new_addr;
}

void free(void *addr)
{
	int params[2];
	params[0] = 0x11;
	params[1] = (int) addr;
	__asm__ volatile("int	$0x83" :: "a"(params));
}

void memcpy(void *from, void *to, int n)
{
	u8 *t = (u8 *) to;
	u8 *f = (u8 *) from;
	for (int i = 0; i < n; i++)
	{
		*(t + i) = *(f + i);
	}
}


#endif
