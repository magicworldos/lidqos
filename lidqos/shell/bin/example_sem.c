/*
 * system.c
 *
 *  Created on: Nov 26, 2014
 *      Author: lidq
 */

#include <shell/semaphore.h>
#include <shell/unistd.h>
#include <shell/pthread.h>

#define PNUM 		(2)

s_sem sem;

void sell_ticket(int num)
{
	//调用0x82号中断程序，显示一个数字
	int params[2];
	params[0] = 1;
	params[1] = num;
	__asm__ volatile("int	$0x82" :: "a"(params));
}

void myfunc(void *args)
{
	int *num = (int *) args;

	while (1)
	{
		//sem_wait(&sem);

		if ((*num) <= 0)
		{
			break;
		}
		msleep(10);
		sell_ticket(*num);
		(*num)--;

		//sem_post(&sem);
	}
	//sem_post(&sem);

	for (;;)
	{

	}
}

int main(int argc, char **args)
{
	//sem_init(&sem, 1);

	int num = 20;

	s_pthread p[PNUM];

	for (int i = 0; i < PNUM; i++)
	{
		pthread_create(&p[i], &myfunc, &num);
	}

	for (;;)
	{
	}

	return 0;
}
