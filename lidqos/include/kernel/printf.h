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
#include <kernel/sys_var.h>

void draw_cursor(int tty_id, int x, int y);

void clear_cursor(int tty_id, int x, int y);

void twinkle_cursor(int flag);

void set_cursor(int tty_id, u32 x, u32 y);

u32 get_cursor(int tty_id);

void scroll_up(int tty_id, int row);

void putascii(int tty_id, u32 x, u32 y, char ch);

void putchar(int tty_id, char ch);

void backspace(int tty_id);

void number_to_str(char *buff, int number, int hex);

int puts(int tty_id, char *str);

int printf(int tty_id, char *fmt, ...);

#endif

#endif
