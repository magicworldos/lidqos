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

	free(p);
	free(p2);

	int num = 4;
	void *ps[num];
	for (int i = 0; i < num; i++)
	{
		ps[i] = malloc(0xe00);
		printf("%x\n", ps[i]);
	}
	for (int i = 0; i < num; i++)
	{
		free(ps[i]);
	}
	u8 *addr = malloc(8);
	printf("%x\n", addr);
	for (int i = 0; i < 8; i++)
	{
		addr[i] = (8 - i);
	}
	u8 *new_addr = realloc(addr, 16);
	printf("%x\n", new_addr);
	for (int i = 0; i < 8; i++)
	{
		printf("%x ", new_addr[i]);
	}
	printf("\n");
	free(new_addr);

	for (;;)
	{
	}

	return 0;
}
