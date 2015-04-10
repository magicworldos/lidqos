/*
 * io.h
 * Copyright (C) Feb 10, 2014 by lidq
 *
 * 端口输入输出头文件
 */

#ifndef _IO_H_
#define _IO_H_

#include <kernel/typedef.h>

/***
 * 关中断
 */
#define cli()	\
	({__asm__ volatile("cli");})

/***
 * 打中断
 */
#define sti()	\
	({__asm__ volatile("sti");})


/***
 * 载入gdt
 */
#define load_gdt(gdtp)	\
	({	\
		__asm__ volatile("lgdt  %0" :: "m"(gdtp));	\
	})

/***
 * 载入idt
 */
#define load_idt(idtp)	\
	({	\
		__asm__ volatile("lidt  %0"::"m"(idtp));	\
	})

/***
 * 向端口写一个字节
 */
static inline void outb_p(u8 val, u16 port)
{
	__asm__ volatile("outb	%0, %1" : : "a" (val), "dN" (port));
}

/**
 * 从端口中读入一个字节
 */
static inline u8 inb_p(u16 port)
{
	u8 val;
	__asm__ volatile("inb	%%dx, %%al" :"=a"(val) : "dx"(port));
	return val;
}

static inline u16 ds()
{
	u16 ds;
	__asm__ volatile("movw	%%ds, %0" : "=a" (ds) : );
	return ds;
}

#endif
