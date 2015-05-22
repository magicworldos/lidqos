/*
 * system.c
 *
 *  Created on: Nov 26, 2014
 *      Author: lidq
 */

#include <shell/unistd.h>

int main(int argc, char **args)
{
	msleep(100);

	char *msg2 = "Example: sleep 3000ms.";
	__asm__ volatile("int	$0x82" :: "a"(msg2));
	msleep(3000);

	char *msg3 = "Example: sleep 1s.";
	for (int i = 0; i < 5; i++)
	{
		__asm__ volatile("int	$0x82" :: "a"(msg3));
		msleep(1000);
	}

	for (;;)
	{
	}

	return 0;
}
