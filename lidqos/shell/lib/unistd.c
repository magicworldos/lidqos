/*
 * unistd.c
 *
 *  Created on: May 22, 2015
 *      Author: lidq
 */

#include <shell/unistd.h>

void msleep(int ms)
{
	int params[2];
	params[0] = 2;
	params[1] = ms;
	__asm__ volatile("int	$0x80" :: "a"(params));
}

void sleep(int s)
{
	msleep(s * 1000);
}
