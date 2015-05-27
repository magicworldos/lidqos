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
		float i = 36.83;
		float j = 7.19;
		float k = i / j;
		printf("%f / %f = %f\n", i, j, k);
	}
	return 0;
}
