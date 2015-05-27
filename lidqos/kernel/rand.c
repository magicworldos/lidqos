/*
 * rand.c
 *
 *  Created on: May 26, 2015
 *      Author: lidq
 */

#include <kernel/rand.h>

//随机数种子
u32 rand_seed = 1;

int rand()
{
	rand_seed = rand_seed * 1103515245 + 12345;
	return (u32) (rand_seed / 65536) % 32768;
}

void srand(u32 seed)
{
	rand_seed = seed;
}

int random(int min, int max)
{
	return rand() % ((max + 1) - min) + min;
}
