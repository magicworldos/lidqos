/*
 * sys_var.h
 *
 *  Created on: Dec 1, 2014
 *      Author: lidq
 */

#ifndef _SYS_VAR_H_
#define _SYS_VAR_H_

#include <kernel/typedef.h>

//tty数量
#define TTY_COUT		(10)
//默认tty编号
#define TTY_DEF_ID		(0)

//ldt代码段选择子
#define USER_CODE_SEL			0x07
//ldt数据段选择子
#define USER_DATA_SEL			0x0f

//默认的不需要执行的任务
#define pcb_TYPE_SPE			(0x0)
//普通可执行任务
#define pcb_TYPE_SYSTEM			(0x1)
#define pcb_TYPE_SERVICE		(0x2)
#define pcb_TYPE_USER			(0x4)

//任务状态：运行
#define T_S_RUNNING		(0)
//任务状态：就绪
#define T_S_READY		(1)
//任务状态：阻塞
#define T_S_WAIT		(2)
//任务状态：停止
#define T_S_STOP		(3)

//任务堆栈大小
#define TASK_STACK_SIZE			(0x40000)

//浮点数据缓冲区大小
#define FPU_SIZE				(0x200)


//tty数据结构
typedef struct
{
	//tty编号
	int tty_id;
	//显存地址
	u8 *mm_addr;
	//光标位置
	int cursor_pos;
	//getchar
	char *ch_addr;
	//getkey
	u8 *scancode;
	//pcb_shell
	void *pcb_shell;
} s_tty;

typedef struct
{
	s_tty **ttys;
	s_tty *tty;
	int tty_ch_id;

} s_tty_info;

typedef struct
{
	//按键左shift
	char kb_key_shift;
	//按键左ctrl
	char kb_key_ctrl;
	//按键左alt
	char kb_key_alt;
	//扫描码
	u8 *keys;
} s_keyboard;

//tss数据结构
typedef struct tss_s
{
	u32 back_link;
	u32 esp0, ss0;
	u32 esp1, ss1;
	u32 esp2, ss2;
	u32 cr3;
	u32 eip;
	u32 eflags;
	u32 eax, ecx, edx, ebx;
	u32 esp, ebp;
	u32 esi, edi;
	u32 es, cs, ss, ds, fs, gs;
	u32 ldt;
	u32 trace_bitmap;
} s_tss;

//tss多任务数据结构
typedef struct task_s
{
	s_tss tss;
	s_gdt ldt[2];
} s_task;

typedef struct alloc_list_s
{
	void* addr;
	int size;
	struct alloc_list_s *next;

} s_alloc_list;

typedef struct process_control_block
{
	//pcb id
	int process_id;
	//pcb type
	int type;
	//执行参数
	char *params;
	//状态
	int status;
	//栈大小
	int stack_size;
	//程序地址
	u32 program_addr;
	//程序大小
	int program_size;
	//esp地址
	u32 stack_esp;
	//esp0地址
	u32 stack_esp0;
	//tty
	s_tty *tty;
	//浮点寄存器数据保存区
	char fpu_data[512];

	s_task *task;
	s_alloc_list *alloc_list;
} s_pcb;

typedef struct
{
	//header
	s_pcb *ph;
	//current
	s_pcb *pc;
	//run
	s_pcb *pr;
	//last run
	s_pcb *plr;
} s_pcb_info;

typedef struct s_hda_rw
{
	u32 lba;
	u8 com;
	u8* buff;
	int* status;

	struct s_hda_rw *next;
} s_hda_rw;

typedef struct s_partition_table
{
	u8 device;
	u8 boot;
	u8 id;
	u8 type;
	u32 bitmap_start;
	u32 start;
	u32 size;
} s_pt;

typedef struct
{
	s_hda_rw *hda_rw_header;
	s_hda_rw *hda_rw_footer;

	int pt_count;
	s_pt *pts;
} s_hda_info;

typedef struct
{
	s_tty_info *tty_info;
	s_keyboard *keyboard_info;
	s_pcb_info *pcb_info;
	s_hda_info *hda_info;

	//shell程序运行前置信号量
	//在shell程序启动前有多少个前置任务
	//当shell_pre_pnum为0时，才启动shell
	int shell_pre_pnum;

} s_sys_var;

#endif /* SYS_VAR_H_ */
