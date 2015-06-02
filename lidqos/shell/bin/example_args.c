/*
 * system.c
 *
 *  Created on: Nov 26, 2014
 *      Author: lidq
 */

#include <shell/stdio.h>

int main(int argc, char **args)
{
	printf("Example args Start:\n");

	for (int i = 0; i < argc; i++)
	{
		printf("%d. %s\n", i, args[i]);
	}

	printf("Example finished.\n");

	return 0;
}
