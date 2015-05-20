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
int factorial(int n)
{
	if (n == 0)
	{
		return 1;
	}
	return n * factorial(n - 1);
}

int main(int argc, char **args)
{
	int n = factorial(6);
	__asm__ volatile ("int $0x80" :: "a"(&n));
	for (;;)
	{
	}

	return 0;
}

#endif
