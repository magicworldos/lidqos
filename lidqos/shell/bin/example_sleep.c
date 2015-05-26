/*
 * system.c
 *
 *  Created on: Nov 26, 2014
 *      Author: lidq
 */

#include <shell/stdio.h>
#include <shell/unistd.h>

int main(int argc, char **args)
{
	for (int i = 1; i <= 5; i++)
	{
		msleep(1000);
		printf("Example: sleep %ds.\n", i);
	}

	return 0;
}
