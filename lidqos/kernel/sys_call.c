/*
 * sys_call.c
 * Copyright (C) Feb 10, 2014 by lidq
 *
 * 系统中断程序
 *  - 处理异常中断程序
 *  - 处理系统中断
 */

#ifndef _SYS_CALL_C_
#define _SYS_CALL_C_

#include <kernel/sys_call.h>

extern s_pcb *pcb_current;
//可显字符
extern u8 keys[0x53][2];
//shift键按下状态
u8 kb_key_shift = 0;

int temp = 0;

/*
 * 除零错
 */
void int_div_error()
{
	printf("int_div_error.\n");
	hlt();
}

/***
 * 调试异常
 */
void int_debug_error()
{
	printf("int_debug_error.\n");
	hlt();
}

/***
 * 不可屏蔽中断
 */
void int_nmi()
{
	printf("int_nmi.\n");
	hlt();
}

/***
 * 断电
 */
void int_power_down()
{
	printf("int_power_down.\n");
	hlt();
}

/***
 * 上溢出
 */
void int_bound_out()
{
	printf("int_bound_out.\n");
	hlt();
}

/***
 * 边界检查
 */
void int_bound_check()
{
	printf("int_bound_check.\n");
	hlt();
}

/*
 * 无效操作码
 */
void int_invalid_opcode()
{
	printf("int_invalid_opcode.\n");
	hlt();
}

/***
 * 没有浮点运算器
 */
void int_no_fpu()
{
	printf("int_no_fpu.\n");
	hlt();
}

/***
 * 双重错误
 */
void int_double_error()
{
	printf("int_double_error.\n");
	hlt();
}

/***
 * 浮点运算器段超限
 */
void int_fpu_out()
{
	printf("int_fpu_out.\n");
	hlt();
}

/***
 * 无效TSS
 */
void int_tss_error()
{
	printf("int_tss_error.\n");
	hlt();
}

/***
 * 无效段
 */
void int_section_error()
{
	printf("int_section_error.\n");
	hlt();
}

/***
 * 无效栈
 */
void int_stack_error()
{
	printf("int_stack_error.\n");
	hlt();
}

/*
 * 一般保护错误
 */
void int_protection_error()
{
	printf("int_protection_error.\n");
	hlt();
}

/***
 * 页错误
 */
void int_page_error()
{
	printf("int_page_error.\n");

	page_error();
}

/***
 * 浮点运算器错误
 */
void int_fpu_error()
{
	printf("int_fpu_error.\n");
	hlt();
}

/*
 * 时钟中断
 */
void int_timer()
{
	//通知PIC可以接受新中断
	outb_p(0x20, 0x20);

	//任务调度算法
	schedule();
}

/*
 * 键盘中断
 */
void int_keyboard()
{
	//取得扫描码
	u8 scan_code = inb_p(0x60);
	//取得按下、抬起状态
	u8 status = scan_code >> 7;
	//扫描码的索引
	u8 key_ind = scan_code & 0x7f;
	//shift按下
	if ((key_ind == KEY_LEFT_SHIFT || key_ind == KEY_RIGHT_SHIFT) && status == 0)
	{
		kb_key_shift = 0x1;
	}
	//shift抬起
	else if ((key_ind == KEY_LEFT_SHIFT || key_ind == KEY_RIGHT_SHIFT) && status == 1)
	{
		kb_key_shift = 0x0;
	}
	else if (status == 0)
	{
		//显示字符
		putchar(keys[key_ind - 1][kb_key_shift]);
	}
	//清除键盘状态可以接受新按键
	outb_p(scan_code & 0x7f, 0x61);
	//通知PIC1可以接受新中断
	outb_p(0x20, 0x20);
}

void int_0x80()
{
//	char *p = (char *) 0xb8000;
//	*p = 'A';
	putchar('A');
}

#endif
