/*
 * system.c
 *
 *  Created on: Nov 26, 2014
 *      Author: lidq
 */

#ifndef _SYSTEM_C_
#define _SYSTEM_C_

#include <shell/system.h>

//int fun(int f);
//
int fun(int f)
{
	if (f == 0)
	{
		return 1;
	}
	return f * fun(f - 1);
}

int main(int argc, char **args)
{
//	char *p = (char *) 0xfffff000;
//	for (int i = 0; i < 100; i++)
//	{
////		for (int j = 0; j < 1024; j++)
////		{
////			p -= 4;
////			*p = 'A';
////		}
//		p -= 0x1000;
//		*p = 'A';
//	}

	int a = fun(7);
	__asm__ volatile ("int $0x80" :: "a"(&a));
	for (;;)
	{
		//__asm__ volatile ("int $0x80");
	}

	return 0;
}

#endif
