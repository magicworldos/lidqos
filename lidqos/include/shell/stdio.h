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
#include <kernel/math.h>
#include <kernel/string.h>
#include <shell/semaphore.h>

void putch(char ch);

void putchar(char ch);

int puts(char *str);

void backspace();

void gets(char *str);

void backspace();

void number_to_str(char *buff, int number, int hex);

void printf_lf_e(int float_or_double, double val, int e, int sign);

void printf_lf(int float_or_double, double val, int e, int sign);

void printf_vf(float val, int with_e);

void printf_vlf(double val, int with_e);

int printf(char *fmt, ...);

char getchar();

int get_int();

void scanf(char *fmt, ...);

#endif /* INCLUDE_SHELL_STDIO_H_ */
