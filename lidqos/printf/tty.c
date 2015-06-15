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

		sys_var->ttys[i].ch_index_w = 0;
		sys_var->ttys[i].ch_index_r = 0;
		sys_var->ttys[i].sem_ch_buff_w.value = TTY_KEY_BUFF_SIZE;
		sys_var->ttys[i].sem_ch_buff_w.list_block = NULL;
		sys_var->ttys[i].sem_ch_buff_r.value = 0;
		sys_var->ttys[i].sem_ch_buff_r.list_block = NULL;

		for (int j = 0; j < TTY_KEY_BUFF_SIZE; j++)
		{
			sys_var->ttys[i].ch[j] = 0;
			sys_var->ttys[i].scan_code[j] = 0;
		}

		u8 *p = sys_var->ttys[i].mm_addr;
		for (int j = 0; j < TTY_MEM_SIZE; j++)
		{
			p[j] = 0;
		}
	}
	tty_cur = &sys_var->ttys[0];

	//告诉地址寄存器要接下来要使用14号寄存器
	outb_p(14, 0x03d4);
	//向光标位置高位寄存器写入值
	outb_p((TTY_MEM_SIZE >> 8) & 0xff, 0x03d5);
	//告诉地址寄存器要接下来要使用15号寄存器
	outb_p(15, 0x03d4);
	//向光标位置高位寄存器写入值
	outb_p(TTY_MEM_SIZE & 0xff, 0x03d5);
}

void switch_tty(int tty_id)
{
	if (tty_id != tty_cur->tty_id)
	{
		//切换tty之前将显存数据保存到原tty内存区
		mmcopy((void *) 0xb8000, sys_var->ttys[tty_cur->tty_id].mm_addr, TTY_MEM_SIZE);
		//切换tty
		tty_cur = &sys_var->ttys[tty_id];
		//切换tty之后将新tty内存区数据更新至显存
		mmcopy(tty_cur->mm_addr, (void *) 0xb8000, TTY_MEM_SIZE);
		u32 x = tty_cur->cursor_pos % 80;
		u32 y = tty_cur->cursor_pos / 80;
		set_cursor(tty_cur->tty_id, x, y);

		tty_cur->ch_index_w = 0;
		tty_cur->ch_index_r = 0;

	}
}
