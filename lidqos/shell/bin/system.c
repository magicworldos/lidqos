/*
 * system.c
 *
 *  Created on: Nov 26, 2014
 *      Author: lidq
 */

#ifndef _SYSTEM_C_
#define _SYSTEM_C_

#include <shell/system.h>

unsigned int g1 = 0x1111;
unsigned int g2 = 0x2222;
unsigned int g3 = 0x3333;
unsigned int g4 = 0x4444;

char *str1 = "aaaa";
char *str2 = "bbbb";

void test(char *p);

int main(int argc, char **args)
{
	g1 = 0x5555;
	g2 = 0x6666;
	g3 = 0x7777;
	g4 = 0x8888;

	test(str1);
	test(str2);

	for (;;)
	{
	}

	return 0;
}

#endif
