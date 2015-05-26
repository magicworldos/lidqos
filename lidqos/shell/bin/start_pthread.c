/*
 * start_pthread.c
 *
 *  Created on: May 26, 2015
 *      Author: lidq
 */


extern int pthread_function(void *args);

void start_pthread()
{
	void *args = (void *)0x1122;
	pthread_function(args);

	int params[2];
	params[0] = 4;
	__asm__ volatile("int	$0x80" :: "a"(params));

	for (;;)
	{
	}
}
