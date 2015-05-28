/*
 * system.c
 *
 *  Created on: Nov 26, 2014
 *      Author: lidq
 */

#include <shell/system.h>

s_sys_var *sys_var = NULL;

int main(int argc, char **args)
{
	printf("Start System Process.\n");

	void *p = malloc(0x1000);
	printf("%x\n", p);

	void *p2 = malloc(0x1000);
	printf("%x\n", p2);

	void *p3 = malloc(0x1000);
	printf("%x\n", p3);

	void *p4 = malloc(0x1000);
	printf("%x\n", p4);

	free(p);
	free(p2);
	free(p3);
	free(p4);

	printf("%x\n", malloc(0x1000));

	for (;;)
	{
	}

	return 0;
}
