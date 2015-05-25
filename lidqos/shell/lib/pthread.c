/*
 * pthread.c
 *
 *  Created on: May 25, 2015
 *      Author: lidq
 */

#include <shell/pthread.h>

void pthread_create(s_pthread *p, void *function, void *args)
{
	int params[4];
	params[0] = 3;
	params[1] = (int) p;
	params[2] = (int) function;
	params[3] = (int) args;
	__asm__ volatile("int	$0x80" :: "a"(params));
}
