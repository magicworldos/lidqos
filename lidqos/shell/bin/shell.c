/*
 * shell.c
 *
 *  Created on: Nov 26, 2014
 *      Author: lidq
 */

#include <shell/shell.h>

int main(int argc, char **args)
{
	char *buff = malloc(0x200);
	char ch = 0;

	printf("\nlidq-os login: ");
	scanf("%s", buff);
	printf("Password: ");
	while (ch != '\n')
	{
		ch = getchar();
	}

	do
	{
		printf("\n[lidq-os /]$ ");
		scanf("%s", buff);
		execute_cmd("/usr/bin/example_fpu");
	}
	while (1);

	return 0;
}

void execute_cmd(char *cmd)
{
	install_program(cmd, "");
}

void install_program(char *path, char *args)
{
	int params[3];
	params[0] = 0;
	params[1] = (int) path;
	params[2] = (int) args;
	__asm__ volatile("int $0x80" :: "a"(params));

}
