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

//GDT全局描述符
s_gdt gdts[GDT_MAX_SIZE];
//GDT全局描述符
s_gdtp gdtp;
//IDT全局描述符
s_idt idts[IDT_MAX_SIZE];
//IDT全局描述符
s_idtp idtp;

/*
 * install_gdt : 安装GDT全局描述符
 * return : void
 */
void install_gdt()
{
	// 0x0
	gdts[0].gdt = 0x0;
	gdts[0].gdt2 = 0x0;
	// 0x8
	gdts[1].gdt = 0x0;
	gdts[1].gdt2 = 0x0;

	// 0x10
	//设置kernel的全局描述符
	u32 kernel_addr = 0x0;
	addr_to_gdt(GDT_TYPE_CS, kernel_addr, &gdts[2], GDT_G_KB, 0xfffff);

	// 0x18
	//设置kernel data的全局描述符
	u32 kernel_data = 0x0;
	addr_to_gdt(GDT_TYPE_DS, kernel_addr, &gdts[3], GDT_G_KB, 0xfffff);

	//设置gdt描述符
	gdtp.gdt_lenth = sizeof(s_gdt) * GDT_MAX_SIZE - 1;
	gdtp.gdt_addr2 = (0x0 + (u32) gdts) >> 16;
	gdtp.gdt_addr = (u32) (0x0 + gdts) & 0xffff;

	//载入GDT全局描述符
	load_gdt(gdtp);

	//设置数据寄存器的GDT选择子
	set_ds(GDT_INDEX_KERNEL_DS);
	set_es(GDT_INDEX_KERNEL_DS);
	set_ss(GDT_INDEX_KERNEL_DS);
	set_fs(GDT_INDEX_KERNEL_DS);
	set_gs(GDT_INDEX_KERNEL_DS);
}

/*
 * addr_to_gdt : 将32位物理地址转为gdt描述符
 *  - u32 addr: 物理地址
 *  - s_gdt *gdt: GDT描述符
 *  -u8 cs_ds: 0为cs 1为ds
 *  return : void
 */
void addr_to_gdt(u8 gdt_type, u32 addr, s_gdt *gdt, u8 limit_type, u32 limit)
{
	//GDT代码段
	if (gdt_type == GDT_TYPE_CS)
	{
		gdt->limit = limit & 0xffff;
		gdt->baseaddr = addr & 0xffff;
		gdt->baseaddr2 = (addr >> 16) & 0xff;
		gdt->p_dpl_type_a = 0x9a;
		gdt->uxdg_limit2 = 0x40 | ((limit >> 16) & 0xf);
		if (limit_type == GDT_G_KB)
		{
			gdt->uxdg_limit2 |= 0x80;
		}
		gdt->baseaddr3 = (addr >> 24) & 0xff;
	}
	//GDT数据段
	else if (gdt_type == GDT_TYPE_DS)
	{
		gdt->limit = limit & 0xffff;
		gdt->baseaddr = addr & 0xffff;
		gdt->baseaddr2 = (addr >> 16) & 0xff;
		gdt->p_dpl_type_a = 0x92;
		gdt->uxdg_limit2 = 0x40 | ((limit >> 16) & 0xf);
		if (limit_type == GDT_G_KB)
		{
			gdt->uxdg_limit2 |= 0x80;
		}
		gdt->baseaddr3 = (addr >> 24) & 0xff;
	}
	//LDT代码段
	else if (gdt_type == LDT_TYPE_CS)
	{
		gdt->limit = limit & 0xffff;
		gdt->baseaddr = addr & 0xffff;
		gdt->baseaddr2 = (addr >> 16) & 0xff;
		gdt->p_dpl_type_a = 0xfa;
		gdt->uxdg_limit2 = 0x40 | ((limit >> 16) & 0xf);
		if (limit_type == GDT_G_KB)
		{
			gdt->uxdg_limit2 |= 0x80;
		}
		gdt->baseaddr3 = (addr >> 24) & 0xff;
	}
	//LDT数据段
	else if (gdt_type == LDT_TYPE_DS)
	{
		gdt->limit = limit & 0xffff;
		gdt->baseaddr = addr & 0xffff;
		gdt->baseaddr2 = (addr >> 16) & 0xff;
		gdt->p_dpl_type_a = 0xf2;
		gdt->uxdg_limit2 = 0x40 | ((limit >> 16) & 0xf);
		if (limit_type == GDT_G_KB)
		{
			gdt->uxdg_limit2 |= 0x80;
		}
		gdt->baseaddr3 = (addr >> 24) & 0xff;
	}
	//tss任务段
	else if (gdt_type == GDT_TYPE_TSS)
	{
		gdt->limit = limit & 0xffff;
		gdt->baseaddr = addr & 0xffff;
		gdt->baseaddr2 = (addr >> 16) & 0xff;
		gdt->p_dpl_type_a = 0x89;
		gdt->uxdg_limit2 = ((limit >> 16) & 0xf);
		if (limit_type == GDT_G_KB)
		{
			gdt->uxdg_limit2 |= 0x80;
		}
		gdt->baseaddr3 = (addr >> 24) & 0xff;

	}
	//ldt局部描述符段
	else if (gdt_type == GDT_TYPE_LDT)
	{
		gdt->limit = limit & 0xffff;
		gdt->baseaddr = addr & 0xffff;
		gdt->baseaddr2 = (addr >> 16) & 0xff;
		gdt->p_dpl_type_a = 0x82;
		gdt->uxdg_limit2 = ((limit >> 16) & 0xf);
		if (limit_type == GDT_G_KB)
		{
			gdt->uxdg_limit2 |= 0x80;
		}
		gdt->baseaddr3 = (addr >> 24) & 0xff;
	}
}

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
	//设置ISR地址的0-15位
	idt->offset = addr;
	//设置ISR选择子
	idt->selector = selector;
	//保留
	idt->bbb_no_use = 0x00;
	//P与DPL
	idt->p_dpl_bbbbb = 0x8e;
	//设置ISR地址的16-31位
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
		addr_to_idt(GDT_INDEX_KERNEL_CS, addr, &idts[i]);
	}

	//设置0x0 ~ 0x2f的中断程序
	for (int i = 0; i < ISR_COUNT; i++)
	{
		addr = (u32) _isr[i];
		addr_to_idt(GDT_INDEX_KERNEL_CS, addr, &idts[i]);
	}

	//从0x80开始为系统中断
	for (int i = ISR_SYSCALL_START; i < ISR_SYSCALL_START + ISR_SYSCALL_COUNT; i++)
	{
		addr = (u32) _isr[i - ISR_EMPTY];
		addr_to_idt_syscall(GDT_INDEX_KERNEL_CS, addr, &idts[i]);
	}

	//设置IDT中断描述符
	idtp.idt_lenth = IDT_MAX_SIZE * sizeof(s_idt) - 1;
	idtp.idt_addr2 = ((u32) idts) >> 16;
	idtp.idt_addr = (u32) idts;

	//载入IDT中断描述符
	load_idt(idtp);
}

/*
 * install_timer : 安装时钟中断
 * return : void
 */
void install_timer()
{
	//计算divisor
	u32 divisor = 1193180 / TIMER_FREQUENCY;
	//写入8253的控制寄存器
	outb_p(0x36, 0x43);
	//写入频率低8位到计数器0
	outb_p(divisor & 0xff, 0x40);
	//写入频率高8位到计数器0
	outb_p(divisor >> 8, 0x40);
	//打开PIC的时钟中断IRQ0
	outb_p(inb_p(0x21) & 0xfe, 0x21);
}

/*
 * install_kb : 安装键盘中断
 * return : void
 */
void install_kb()
{
	//打开IRQ1的键盘中断
	outb_p(inb_p(0x21) & 0xfd, 0x21);
	//清除键盘状态可以接受新按键
	outb_p(0x7f, 0x61);
}

/*
 * mmcopy : 复制内存区域
 *  - void *from : 源地址
 *  - void *to : 目的地址
 *  - u32 n : 复制大小
 * return : void
 */
void mmcopy(void *from, void *to, u32 n)
{
	u8 *t = (u8 *) to;
	u8 *f = (u8 *) from;
	for (u32 i = 0; i < n; i++)
	{
		*(t + i) = *(f + i);
	}
}

/*
 * mmcopy_with : 按一定条件复制内存区域
 *  - : 源地址
 *  - : 目的地址
 *  - : 起始偏移量
 *  - : 复制大小
 * return : void
 */
void mmcopy_with(void *from, void *to, int offset, int size)
{
	u32 i;
	u8 *d = (u8 *) to;
	u8 *s = (u8 *) from;
	for (i = 0; i < size; i++)
	{
		*(d + i) = *(s + offset + i);
	}
}

#endif
