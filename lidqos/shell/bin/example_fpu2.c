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
	for (;;)
	{
		float i = 6.14;
		float j = 23.78;
		int k = (int) (i * j);

		printf("B: %d\n", k);
	}
	return 0;
}
