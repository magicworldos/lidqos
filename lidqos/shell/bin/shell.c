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

	do
	{
		printf("\nlidq-os login: ");
		scanf("%s", buff);
		printf("Password: ");
		while (ch != '\n')
		{
			ch = getchar();
		}
		ch = 0;
	}
	while (1);

	return 0;
}
