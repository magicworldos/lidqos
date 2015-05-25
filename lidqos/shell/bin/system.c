/*
 * system.c
 *
 *  Created on: Nov 26, 2014
 *      Author: lidq
 */

#include <shell/system.h>

int main(int argc, char **args)
{
	char *str = "Start System Process.";
	int params[2];
	params[0] = 0;
	params[1] = (int) str;
	__asm__ volatile("int	$0x82" :: "a"(params));

	for (;;)
	{
	}

	return 0;
}
