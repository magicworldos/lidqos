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
		float k = i * j;
		printf("%f x %f = %f\n", i, j, k);
	}
	return 0;
}
