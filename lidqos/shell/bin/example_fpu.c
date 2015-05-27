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
	for (;;)
	{
		float i = 3.14;
		float j = 5.78;
		int k = (int) (i * j);

		printf("A: %d\n", k);
	}

	return 0;
}
