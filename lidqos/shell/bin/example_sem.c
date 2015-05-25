/*
 * system.c
 *
 *  Created on: Nov 26, 2014
 *      Author: lidq
 */

#include <shell/semaphore.h>

int main(int argc, char **args)
{
	s_sem sem;
	sem_init(&sem, 1);

	sem_wait(&sem);
	sem_post(&sem);

	char *msg = "example_sem.c";
	__asm__ volatile("int	$0x82" :: "a"(msg));

	for (;;)
	{
	}

	return 0;
}
