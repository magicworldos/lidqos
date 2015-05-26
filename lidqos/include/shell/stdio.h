/*
 * stdio.h
 *
 *  Created on: May 26, 2015
 *      Author: lidq
 */

#ifndef _INCLUDE_SHELL_STDIO_H_
#define _INCLUDE_SHELL_STDIO_H_

#include <kernel/sys_var.h>
#include <kernel/arg.h>
#include <shell/semaphore.h>

void putch(char ch);

void putchar(char ch);

int puts(char *str);

void number_to_str(char *buff, int number, int hex);

int printf(char *fmt, ...);

char getchar();

#endif /* INCLUDE_SHELL_STDIO_H_ */
