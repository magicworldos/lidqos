/*
 * system.c
 *
 *  Created on: Nov 26, 2014
 *      Author: lidq
 */

#include <shell/stdio.h>
#include <shell/unistd.h>
#include <kernel/io.h>

int main(int argc, char **args)
{
	float i = 3.14;
	float j = 5.78;
	float k = i * j;


//	outb_p(14, 0x03d4);
//	outb_p((2 >> 8) & 0xff, 0x03d5);
//	outb_p(15, 0x03d4);
//	outb_p(2 & 0xff, 0x03d5);

	printf("FPU: %f x %f = %f\n", i, j, k);

	return 0;
}
