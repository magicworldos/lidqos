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

void manager(int total)
{
	int checked_num = 0;
	while (checked_num < total)
	{

	}
}

int main(int argc, char **args)
{
	for (int i = 0; i < 100; i++)
	{
		printf("%d ", random(0, 1));
	}

	return 0;
}
