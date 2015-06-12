/*
 * curses.h
 *
 *  Created on: Jun 12, 2015
 *      Author: lidq
 */

#ifndef _INCLUDE_SHELL_CURSES_H_
#define _INCLUDE_SHELL_CURSES_H_

#include <kernel/typedef.h>
#include <kernel/sys_var.h>

void initscr();

void move(int x, int y);

void getxy(int *x, int *y);

void addch(char ch);

void addstr(char *str);

void clear();

void endwin();

#endif /* INCLUDE_SHELL_CURSES_H_ */
