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
	__asm__ volatile("int	$0x82" :: "a"(str));

	for (;;)
	{
	}

	return 0;
}
