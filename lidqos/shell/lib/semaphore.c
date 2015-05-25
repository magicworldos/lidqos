/*
 * semaphore.c
 *
 *  Created on: May 25, 2015
 *      Author: lidq
 */

#include <shell/semaphore.h>

void sem_init(s_sem *sem, int value)
{
	sem->value = value;
	sem->list_block = NULL;
}

void sem_wait(s_sem *sem)
{
	int params[2];
	//P
	params[0] = 0;
	params[1] = (int) sem;
	__asm__ volatile("int	$0x81" :: "a"(params));
}

void sem_post(s_sem *sem)
{
	int params[2];
	//V
	params[0] = 1;
	params[1] = (int) sem;
	__asm__ volatile("int	$0x81" :: "a"(params));
}
