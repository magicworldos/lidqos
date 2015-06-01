/*
 * system.c
 *
 *  Created on: Nov 26, 2014
 *      Author: lidq
 */

#include <shell/stdio.h>
#include <shell/semaphore.h>
#include <shell/unistd.h>
#include <shell/pthread.h>

#define NUM 		(10)
#define PNUM 		(10)

//全局售票信号量
s_sem sem;

void sell_ticket(int num)
{
	printf("%d\n", num);
}

void myfunc(void *args)
{
	int *num = (int *) args;
	while (1)
	{
		//信号量P操作
		sem_wait(&sem);
		//剩余票数为0时停止售票
		if ((*num) <= 0 || (*num) > NUM)
		{
			break;
		}
		//模拟等待了一小会
		msleep(10);
		//售票
		sell_ticket(*num);
		//剩余票数减1
		(*num)--;
		//信号量V操作
		sem_post(&sem);
	}
	//信号量V操作
	sem_post(&sem);
}

int main(int argc, char **args)
{
	//初始化信号量
	sem_init(&sem, 1);
	//剩余票数
	int num = NUM;
	s_pthread p[PNUM];
	//创建多个线程
	for (int i = 0; i < PNUM; i++)
	{
		pthread_create(&p[i], &myfunc, &num);
	}

	return 0;
}
