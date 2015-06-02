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

#define MIN_ICECREAM_NUM	(1)
#define MAX_ICECREAM_NUM	(4)
#define CUSTOM_NUM			(10)

int total = CUSTOM_NUM * MAX_ICECREAM_NUM;
int passed = 0;
s_sem requested =
{ 0xf, NULL };
s_sem finished =
{ 0xf, NULL };
s_sem locked =
{ 0xf, NULL };

int cstm_line_num = 0;
s_sem cstm_locked =
{ 0xf, NULL };
s_sem cstm_requested =
{ 0xf, NULL };
s_sem cstm_finished[CUSTOM_NUM] =
{ 0xf, };

typedef struct
{
	int id;
	int ice_num;
} cstm;

cstm cstms[CUSTOM_NUM] =
{ 0xf, };

void manager()
{
	printf("manager start.\n");
	int checked_num = 0;
	while (checked_num < total)
	{
		sem_wait(&requested);
		passed = random(0, 1);
		if (passed > 0)
		{
			checked_num++;
		}
		sem_post(&finished);
	}
	printf("manager done.\n");
}

void clerk(s_sem *ice_done)
{
	int psd = 0;
	while (psd == 0)
	{
		sem_wait(&locked);
		sem_post(&requested);
		sem_wait(&finished);
		psd = passed;
		sem_post(&locked);
	}
	sem_post(ice_done);
	printf("clerk done.\n");
}

void customer(cstm *cm)
{
	s_sem ice_done[cm->ice_num];
	for (int i = 0; i < cm->ice_num; i++)
	{
		sem_init(&ice_done[i], 0);
		pthread_create(NULL, &clerk, &ice_done[i]);
	}

	for (int i = 0; i < cm->ice_num; i++)
	{
		sem_wait(&ice_done[i]);
	}

	sem_wait(&cstm_locked);
	int place = cstm_line_num++;
	sem_post(&cstm_requested);
	sem_wait(&cstm_finished[place]);
	sem_post(&cstm_locked);

	printf("custom%d leave.\n", cm->id);
}

void cashier()
{
	for (int i = 0; i < CUSTOM_NUM; i++)
	{
		sem_wait(&cstm_requested);
		printf("cashout %d\n", i);
		sem_post(&cstm_finished[i]);
	}

	printf("cashier done.\n");
}

int main(int argc, char **args)
{
	printf("example ice cream.\n");

	passed = 0;
	sem_init(&requested, 0);
	sem_init(&finished, 0);
	sem_init(&locked, 1);

	cstm_line_num = 0;

	sem_init(&cstm_locked, 1);
	sem_init(&cstm_requested, 0);

	for (int i = 0; i < CUSTOM_NUM; i++)
	{
		sem_init(&cstm_finished[i], 0);
	}

	pthread_create(NULL, &manager, NULL);

	pthread_create(NULL, &cashier, NULL);

	int ice_num[CUSTOM_NUM];
	int t = 0;
	for (int i = 0; i < CUSTOM_NUM; i++)
	{
		ice_num[i] = random(MIN_ICECREAM_NUM, MAX_ICECREAM_NUM);
		t += ice_num[i];
		cstms[i].id = i;
		cstms[i].ice_num = ice_num[i];
		pthread_create(NULL, &customer, &cstms[i]);
	}
	total = t;
	printf("total %d\n", total);

	return 0;
}
