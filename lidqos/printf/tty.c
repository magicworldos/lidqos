/*
 * tty.c
 *
 *  Created on: Jun 4, 2015
 *      Author: lidq
 */

#include <kernel/tty.h>

extern s_sys_var *sys_var;
s_tty *tty_cur = NULL;

void install_tty()
{
	for (int i = 0; i < TTY_COUNT; i++)
	{
		sys_var->ttys[i].tty_id = i;
		sys_var->ttys[i].cursor_pos = 0;
		sys_var->ttys[i].mm_addr = alloc_mm(TTY_MEM_SIZE);
	}

	tty_cur = &sys_var->ttys[0];
}
