/*
 * example_curses.c
 *
 *  Created on: Jun 12, 2015
 *      Author: lidq
 */

#include <shell/stdio.h>
#include <shell/curses.h>

int main(int argc, char **args)
{
	initscr();

	getchar();

	endwin();

	return 0;
}
