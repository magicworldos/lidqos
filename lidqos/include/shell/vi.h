/*
 * vi.h
 *
 *  Created on: Jun 12, 2015
 *      Author: lidq
 */

#ifndef _INCLUDE_SHELL_VI_H_
#define _INCLUDE_SHELL_VI_H_

#include <kernel/typedef.h>
#include <kernel/key.h>
#include <shell/stdio.h>
#include <shell/fs.h>
#include <shell/path.h>
#include <shell/stdlib.h>
#include <shell/curses.h>
#include <kernel/string.h>

#define MODE_NORMAL		(0)
#define MODE_INSERT		(1)
#define MODE_CMD		(2)

typedef struct
{
	char *line;
	void *next;
} s_vdata;

typedef struct
{
	//25
	int max_row;
	//80
	int max_col;

	int start_row;
	int start_col;

	int x;
	int y;
	u32 status;

} s_vi_data;

int main(int argc, char **args);

void edit_file(char *path, char *data, int size);

s_vdata* convert_vdata(char *data);

void mode_normal();

void mode_insert();

int mode_cmd();

void switch_mode(u8 mode);

void show_status(char *str);

#endif /* INCLUDE_SHELL_VI_H_ */
