/*
 * printf.c
 * Copyright (C) Feb 10, 2014 by lidq
 *
 * 标准输出头文件
 */

#ifndef _PRINTF_H_
#define _PRINTF_H_

#ifndef MK_ISO_FS

#include <kernel/typedef.h>
#include <kernel/io.h>
#include <kernel/string.h>
#include <kernel/arg.h>

void set_cursor(u32 x, u32 y);

u32 get_cursor();

void scroll_up(int row);

void putascii(u32 x, u32 y, char ch);

void putchar(char ch);

void backspace();

int puts(char *str);

int printf(char *fmt, ...);

#endif

#endif
