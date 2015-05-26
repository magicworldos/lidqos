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
	printf("Enter '\\n' to the end of the function 'getchar()'.\n");
	char ch = 0;
	do
	{
		ch = getchar();
		putchar(ch);
	}
	while (ch != '\n');

	printf("Enter \"exit\" to the end of the function 'gets()'.\n");
	char str[0x200];
	do
	{
		gets(str);
		printf("%s\n", str);
	}
	while (str_compare("exit", str) != 0);

	int i;
	do
	{
		printf("Enter 0 for i to the end of the function 'scanf()'.\n");
		printf("i = ");
		scanf("%d", &i);
		printf("ch = ");
		scanf("%c", &ch);
		printf("\n");
		printf("str = ");
		scanf("%s", str);

		printf("i = %d\tch = %c\tstr = %s\n", i, ch, str);
	}
	while (i != 0);

	printf("End of stdio example.\n");
	return 0;
}
