/*
 * pwd.c
 *
 *  Created on: Nov 24, 2014
 *      Author: lidq
 */

#include <shell/stdlib.h>
#include <shell/stdio.h>

int main(int argc, char **args)
{
	//当前工作目录
	char *current_path = malloc(SHELL_CMD_LEN);
	int params[2];
	params[0] = 6;
	params[1] = (int) current_path;
	__asm__ volatile("int $0x80" :: "a"(params));
	printf("%s\n", current_path);
	free(current_path);

	return 0;
}
