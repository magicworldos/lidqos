/*
 * system.c
 *
 *  Created on: Nov 26, 2014
 *      Author: lidq
 */

#include <shell/unistd.h>
#include <shell/stdio.h>

int main(int argc, char **args)
{
	for (int i = 0; i < argc; i++)
	{
		printf("%d. %s\n", i, args[i]);
	}

	return 0;
}
