/*
 * system.c
 *
 *  Created on: Nov 26, 2014
 *      Author: lidq
 */

#ifndef _SYSTEM_C_
#define _SYSTEM_C_

#include <shell/system.h>

int main(int argc, char **args)
{
//	char *p = (char *)0xb8000;
//	p[0] = 'A';
	for (;;)
	{
		__asm__ volatile ("int $0x80");
	}

	return 0;
}

#endif
