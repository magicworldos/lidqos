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
	int ret = 0;
	int params[3];
	//P
	params[0] = 0;
	params[1] = (int) sem;
	params[2] = (int) &ret;
	while (ret == 0)
	{
		__asm__ volatile("int	$0x81" :: "a"(params));
	}
}

void sem_post(s_sem *sem)
{
	int ret = 0;
	int params[3];
	//V
	params[0] = 1;
	params[1] = (int) sem;
	params[2] = (int) &ret;
	while (ret == 0)
	{
		__asm__ volatile("int	$0x81" :: "a"(params));
	}
}

s_sem* get_global_sem(int type)
{
	s_sem *sem;
	int params[3];
	params[0] = 2;
	params[1] = (int) type;
	params[2] = (int) &sem;
	__asm__ volatile("int	$0x81" :: "a"(params));
	return sem;
}
