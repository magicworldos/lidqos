/*
 * cursor.c
 *
 *  Created on: Jun 5, 2015
 *      Author: lidq
 */

#include <kernel/typedef.h>
#include <shell/unistd.h>

int main(int argc, char **args)
{
	int params[2];
	params[0] = 0x20;

	for (u32 i = 0;; i++)
	{
		params[1] = i % 2;
		__asm__ volatile ("int $0x82" :: "a"(params));
		msleep(500);
	}

	return 0;
}
