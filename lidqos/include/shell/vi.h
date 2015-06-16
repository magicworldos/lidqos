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
	int length;
	int mm_length;
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

	int line_count;

	int last_col_no;

} s_vi_data;

int main(int argc, char **args);

void edit_file(char *data, int size);

s_vdata* convert_vdata(char *data);

void insert_char(s_vdata *p, char ch);

void mode_normal();

void mode_insert();

int mode_cmd();

void get_cmd(char *str);

int save();

void switch_mode(u8 mode);

int file_status();

void set_file_status(int changed);

void scroll(int key);

void show_status(char *str);

void show_status_msg(char *str);

void backspace_char();

void delete_char();

void newline_char();

void empty_buff(char *buff, int length);

s_vdata* current_line();

void addstr_with_buff(char *line);

void all_line();

#endif /* INCLUDE_SHELL_VI_H_ */
