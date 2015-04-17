/*
 * mm.h
 * Copyright (C) Feb 10, 2014 by lidq
 *
 * 内存操作头文件
 */

#ifndef _MM_H_
#define _MM_H_

#include <kernel/typedef.h>

//全局描述符个数
#define GDT_MAX_SIZE (0xff)
//全局中断符
#define IDT_MAX_SIZE (0xff)

//内核代码选择子
#define GDT_INDEX_KERNEL_CS		(0x10)
//内核数据选择子
#define GDT_INDEX_KERNEL_DS		(0x18)

//ldt代码段选择子
#define USER_CODE_SEL			0x07
//ldt数据段选择子
#define USER_DATA_SEL			0x0f

//中断程序数
#define ISR_COUNT				(0x30)
//跳过空的中断娄
#define ISR_EMPTY				(0x50)
//系统中断数
#define ISR_SYSCALL_COUNT		(0x20)
//系统中断开始于
#define ISR_SYSCALL_START		(ISR_COUNT + ISR_EMPTY)

//时钟频率100hz
#define TIMER_FREQUENCY			(100)

//GDT类型代码段
#define GDT_TYPE_CS				(0)
//GDT类型数据段
#define GDT_TYPE_DS				(1)
//GDT类型任务段
#define GDT_TYPE_TSS			(2)
//GDT类型LDT段
#define GDT_TYPE_LDT			(3)
//LDT类型代码段
#define LDT_TYPE_CS				(4)
//LDT类型数据段
#define LDT_TYPE_DS				(5)

//GDT长度单位byte
#define GDT_G_BYTE				(0)
//GDT长度单位kb
#define GDT_G_KB				(1)

//tts的全局选择子
#define GDT_INDEX_TSS			0x20
//ldt的全局选择子
#define	GDT_INDEX_LDT			0x28

/*
 * install_gdt : 安装GDT全局描述符
 * return : void
 */
void install_gdt();

/*
 * addr_to_gdt : 将32位物理地址转为gdt描述符
 *  - u32 addr: 物理地址
 *  - s_gdt *gdt: GDT描述符
 *  -u8 cs_ds: 0为cs 1为ds
 *  return : void
 */
void addr_to_gdt(u8 gdt_type, u32 addr, s_gdt *gdt, u8 limit_type, u32 limit);

/*
 * _int_default : 默认中断程序
 * return : void
 */
extern void _int_default();

/*
 * void (*_isr[ISR_COUNT])(void) : 中断程序函数组
 * return : void
 */
extern void (*_isr[ISR_COUNT])(void);

/*
 * addr_to_idt : 将32位物理地址转为IDT描述符
 *  - u16 selector : 选择子
 *  - u32 addr : 中断程序所在的物理地址
 *  - s_idt *idt : 中断描述符
 * return : void
 */
void addr_to_idt(u16 selector, u32 addr, s_idt *idt);

/*
 * addr_to_idt_syscall : 将32位物理地址转为IDT描述符
 *  - u16 selector: 选择子
 *  - u32 addr: 系统中断程序所在的物理地址
 *  - s_idt *idt: 中断描述符
 */
void addr_to_idt_syscall(u16 selector, u32 addr, s_idt *idt);

/*
 * install_idt : 安装IDT全局描述符
 * return : void
 */
void install_idt();

/*
 * install_pic : 安装8259A中断控制器
 * return : void
 */
void install_pic();

/*
 * install_timer : 安装时钟中断
 * return : void
 */
void install_timer();

/*
 * install_kb : 安装键盘中断
 * return : void
 */
void install_kb();

void mmcopy(void *from, void *to, u32 n);

#endif
