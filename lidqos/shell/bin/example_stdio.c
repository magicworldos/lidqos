/*
 * example_stdio.c
 *
 *  Created on: May 26, 2015
 *      Author: lidq
 */

#include <kernel/string.h>
#include <shell/stdio.h>

int main(int argc, char **args)
{
	printf("getchar() '\\n' for end.\n");
	char ch = 0;
	do
	{
		ch = getchar();
		putchar(ch);
	}
	while (ch != '\n');

	printf("gets() \"exit\" for end.\n");
	char buff[0x200];
	do
	{
		gets(buff);
		printf("%s\n", buff);
	}
	while (str_compare("exit", buff) != 0);

	printf("The end.\n");
	return 0;
}
