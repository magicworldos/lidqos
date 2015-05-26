/*
 * example_stdio.c
 *
 *  Created on: May 26, 2015
 *      Author: lidq
 */

#include <shell/stdio.h>

int main(int argc, char **args)
{
	printf("Example: stdio.\n");
	char ch = 0;
	do
	{
		ch = getchar();
		putchar(ch);
	}
	while (ch != '\n');
	printf("The end.\n");
	return 0;
}
