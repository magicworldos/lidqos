/*
 * example_Icecream.c
 *
 *  Created on: May 27, 2015
 *      Author: lidq
 */

#include <shell/stdio.h>
#include <shell/stdlib.h>
#include <shell/semaphore.h>
#include <shell/unistd.h>
#include <shell/pthread.h>

#define CUSTOM_NUM		(10)

int passed;
s_sem requested;
s_sem finished;
s_sem locked;

int cstm_line_num;
s_sem cstm_requested;
s_sem cstm_locked;
s_sem cstm_finished0;
s_sem cstm_finished1;
s_sem cstm_finished2;
s_sem cstm_finished3;
s_sem cstm_finished4;
s_sem cstm_finished5;
s_sem cstm_finished6;
s_sem cstm_finished7;
s_sem cstm_finished8;
s_sem cstm_finished9;

void manager()
{
//	int checked_num = 0;
	while (1)
	{
		printf("manager start check 1.\n");
		sem_wait(&requested);
		printf("manager start check 2.\n");
		if (random(0, 1))
		{
			passed = 1;
//			checked_num++;
		}
		printf("manager finished.\n");
		//sem_post(&finished);
	}
	printf("manager done.\n");
}

//void clerk(s_sem *ice_done)
//{
//	u8 psd = 0;
//	while (!psd)
//	{
//		sem_wait(&locked);
//		sem_post(&requested);
//		printf("wait manager finished\n");
//		sem_wait(&finished);
//		psd = passed;
//		sem_post(&locked);
//	}
//	sem_post(ice_done);
//	printf("clerk done.\n");
//}
//
//void customer()
//{
//	int ice_num = random(1, 4);
//	s_sem ice_done[ice_num];
//	printf("customer book ice_num %d\n", ice_num);
//	for (int i = 0; i < ice_num; i++)
//	{
//		sem_init(&ice_done[i], 0);
//		pthread_create(NULL, &clerk, &ice_done[i]);
//	}
//
//	for (int i = 0; i < ice_num; i++)
//	{
//		sem_wait(&ice_done[i]);
//	}
//
//	printf("custom get icecreams.\n");
//
//	sem_wait(&cstm_locked);
//	int place = cstm_line_num++;
//	sem_post(&cstm_requested);
//	sem_wait(&cstm_finished[place]);
//	sem_post(&cstm_locked);
//
//	printf("custom leave.\n");
//}
//
//void cashier()
//{
//	for (int i = 0; i < CUSTOM_NUM; i++)
//	{
//		sem_wait(&cstm_requested);
//		printf("cashout %d\n", i);
//		sem_post(&cstm_finished[i]);
//	}
//
//	printf("cashier done.\n");
//}

int main(int argc, char **args)
{
	printf("example ice cream.\n");

	passed = 0;
	sem_init(&requested, 0);
	sem_init(&finished, 0);
	sem_init(&locked, 1);

	cstm_line_num = 0;
	sem_init(&cstm_requested, 0);
	sem_init(&cstm_locked, 1);
//	for (int i = 0; i < CUSTOM_NUM; i++)
//	{
//		sem_init(&cstm_finished[i], 0);
//	}
	sem_init(&cstm_finished0, 0);
	sem_init(&cstm_finished1, 0);
	sem_init(&cstm_finished2, 0);
	sem_init(&cstm_finished3, 0);
	sem_init(&cstm_finished4, 0);
	sem_init(&cstm_finished5, 0);
	sem_init(&cstm_finished6, 0);
	sem_init(&cstm_finished7, 0);
	sem_init(&cstm_finished8, 0);
	sem_init(&cstm_finished9, 0);

	pthread_create(NULL, &manager, NULL);

//	pthread_create(NULL, &cashier, NULL);
//
//	for (int i = 0; i < CUSTOM_NUM; i++)
//	{
//		pthread_create(NULL, &customer, NULL);
//	}

	return 0;
}
