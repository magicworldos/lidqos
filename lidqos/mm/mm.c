/*
 * mm.c
 * Copyright (C) Feb 10, 2014 by lidq
 *
 * 内存操作:
 *  - 处理内存初始化和管理内存操作
 */

#ifndef _MM_C_
#define _MM_C_

#include <kernel/typedef.h>
#include <kernel/io.h>
#include <kernel/mm.h>
#include <kernel/printf.h>

//IDT全局描述符
s_idt idts[IDT_MAX_SIZE];
//IDT全局描述符
s_idtp idtp;

/*
 * install_pic : 安装8259A中断控制器
 * return : void
 */
void install_pic()
{
	//设置主8259A和从8259A
	outb_p(0x11, 0x20);
	outb_p(0x11, 0xa0);

	//设置IRQ0-IRQ7的中断向量为0x20-0x27
	outb_p(0x20, 0x21);
	//设置IRQ8-IRQ15的中断向量为0x28-0x2f

	outb_p(0x28, 0xa1);
	//使从片PIC2连接到主片上
	outb_p(0x04, 0x21);
	outb_p(0x02, 0xa1);

	//打开8086模式
	outb_p(0x01, 0x21);
	outb_p(0x01, 0xa1);

	//关闭IRQ0-IRQ7的0x20-0x27中断
	outb_p(0xff, 0x21);
	//关闭IRQ8-IRQ15的0x28-0x2f中断
	outb_p(0xff, 0xa1);
}

/*
 * addr_to_idt : 将32位物理地址转为IDT描述符
 *  - u16 selector : 选择子
 *  - u32 addr : 中断程序所在的物理地址
 *  - s_idt *idt : 中断描述符
 * return : void
 */
void addr_to_idt(u16 selector, u32 addr, s_idt *idt)
{
	idt->offset = addr;
	idt->selector = selector;
	idt->bbb_no_use = 0x00;
	idt->p_dpl_bbbbb = 0x8e;
	idt->offset2 = addr >> 16;
}

/*
 * addr_to_idt_syscall : 将32位物理地址转为IDT描述符
 *  - u16 selector: 选择子
 *  - u32 addr: 系统中断程序所在的物理地址
 *  - s_idt *idt: 中断描述符
 */
void addr_to_idt_syscall(u16 selector, u32 addr, s_idt *idt)
{
	idt->offset = addr;
	idt->selector = selector;
	idt->bbb_no_use = 0x00;
	idt->p_dpl_bbbbb = 0xef;
	idt->offset2 = addr >> 16;
}

/*
 * install_idt : 安装IDT全局描述符
 * return : void
 */
void install_idt()
{
	//地址
	u32 addr;

	//设置所有中断程序为_int_default
	addr = (u32) &_int_default;
	for (int i = 0; i < IDT_MAX_SIZE; i++)
	{
		addr_to_idt(DT_INDEX_KERNEL_CS, addr, &idts[i]);
	}

	//设置0x0 ~ 0x2f的中断程序
	for (int i = 0; i < ISR_COUNT; i++)
	{
		addr = (u32) _isr[i];
		addr_to_idt(DT_INDEX_KERNEL_CS, addr, &idts[i]);
	}

	//从0x80开始为系统中断
	for (int i = ISR_SYSCALL_START; i < ISR_SYSCALL_START + ISR_SYSCALL_COUNT; i++)
	{
		addr = (u32) _isr[i - ISR_EMPTY];
		addr_to_idt(DT_INDEX_KERNEL_CS, addr, &idts[i]);
	}

	//设置IDT中断描述符
	idtp.idt_lenth = IDT_MAX_SIZE * sizeof(s_idt) - 1;
	idtp.idt_addr2 = ((u32) idts) >> 16;
	idtp.idt_addr = (u32) idts;

	//载入IDT中断描述符
	load_idt(idtp);
}

#endif
