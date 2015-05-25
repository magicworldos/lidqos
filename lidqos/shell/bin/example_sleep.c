/*
 * system.c
 *
 *  Created on: Nov 26, 2014
 *      Author: lidq
 */

#include <shell/unistd.h>

int main(int argc, char **args)
{
//	char *msg2 = "Example: sleep 3000ms.";
	int params[2];
	params[0] = 0;
//	params[1] = (int) msg2;
//	__asm__ volatile("int	$0x82" :: "a"(params));
//	msleep(3000);

	char *msg3 = "Example: sleep 1s.";
	for (int i = 0; i < 3; i++)
	{
		params[1] = (int) msg3;
		__asm__ volatile("int	$0x82" :: "a"(params));
		msleep(1000);
	}

	return 0;
}
