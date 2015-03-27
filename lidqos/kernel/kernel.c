/*
 * kernel.c
 *
 *  Created on: Mar 26, 2015
 *      Author: lidq
 */

#include <kernel/kernel.h>

char *str = "Hello World!";

int start_kernel(int argc, char **args)
{
	char *p = (char *) 0xb8000;
	for (int i = 0; str[i] != '\0'; i++)
	{
		p[i * 2] = str[i];
	}

	for (;;)
		;
	return 0;
}
