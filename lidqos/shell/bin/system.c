/*
 * system.c
 *
 *  Created on: Nov 26, 2014
 *      Author: lidq
 */

#ifndef _SYSTEM_C_
#define _SYSTEM_C_

#include <shell/system.h>

int fun(int f)
{
	if (f == 0)
	{
		return 1;
	}
	__asm__ volatile ("int $0x80");
	return f * fun(f - 1);
}

int main(int argc, char **args)
{
//	char *p = (char *)0xb8000;
//	p[0] = 'A';

	int a = fun(0x8000);
	a++;
	for (;;)
	{
		//__asm__ volatile ("int $0x80");
	}

	return 0;
}

#endif
