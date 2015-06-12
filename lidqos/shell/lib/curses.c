/*
 * curses.c
 *
 *  Created on: Jun 12, 2015
 *      Author: lidq
 */

#include <shell/curses.h>
#include <shell/stdlib.h>

typedef struct
{
	u8 mem[TTY_MEM_SIZE];
	int x;
	int y;
} s_curses;

s_curses *curses;

void initscr()
{
	curses = malloc(sizeof(s_curses));

	getxy(&curses->x, &curses->y);

	int params[2];
	params[0] = 0;
	params[1] = (int) curses->mem;
	__asm__ volatile ("int $0x88" :: "a"(params));

	move(0, 0);
}

void endwin()
{
	int params[2];
	params[0] = 1;
	params[1] = (int) curses->mem;
	__asm__ volatile ("int $0x88" :: "a"(params));

	move(curses->x, curses->y);

	free(curses);
}

void move(int x, int y)
{
	int params[3];
	params[0] = 2;
	params[1] = x;
	params[2] = y;
	__asm__ volatile ("int $0x88" :: "a"(params));
}

void getxy(int *x, int *y)
{
	int params[3];
	params[0] = 3;
	params[1] = (int) x;
	params[2] = (int) y;
	__asm__ volatile ("int $0x88" :: "a"(params));
}

void addch(char ch)
{
	int params[2];
	params[0] = 4;
	params[1] = ch;
	__asm__ volatile ("int $0x88" :: "a"(params));
}

void addstr(char *str)
{
	int params[2];
	params[0] = 5;
	params[1] = (int) str;
	__asm__ volatile ("int $0x88" :: "a"(params));
}

void clear()
{
	char *buff = malloc(TTY_MEM_CH_SIZE + 1);
	for (int i = 0; i < TTY_MEM_CH_SIZE; i++)
	{
		buff[i] = ' ';
	}
	buff[TTY_MEM_CH_SIZE] = '\0';
	addstr(buff);
	move(0, 0);
}

