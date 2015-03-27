/*
 * io.h
 * Copyright (C) Feb 10, 2014 by lidq
 *
 * 端口输入输出头文件
 */

#ifndef _IO_H_
#define _IO_H_

#include <kernel/typedef.h>

/*
 * close int
 */
#define cli()	\
	({__asm__ volatile("cli");})

/*
 * open int
 */
#define sti()	\
	({__asm__ volatile("sti");})


/*
 * struct gdt_s
 */
#define load_gdt(gdtp)	\
	({	\
		__asm__ volatile("lgdt  %0" :: "m"(gdtp));	\
	})

static inline void outb_p(u8 val, u16 port)
{
	__asm__ volatile("outb	%0, %1" : : "a" (val), "dN" (port));
}

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
