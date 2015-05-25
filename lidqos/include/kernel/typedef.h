/*
 * typedef.h
 * Copyright (C) Feb 10, 2014 by lidq
 *
 * 系统定义头文件
 */

#ifndef _TYPEDEF_H_
#define _TYPEDEF_H_

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef signed int s32;
typedef unsigned long long u64;

#ifndef NULL
#define NULL 		((void*)(0))
#endif

//GDT全局描述符表
typedef struct
{
	union
	{
		struct
		{
			u64 lgdt;
		};

		struct
		{
			u32 gdt, gdt2;
		};

		struct
		{
			u16 limit;
			u16 baseaddr;
			u8 baseaddr2;
			u8 p_dpl_type_a;
			u8 uxdg_limit2;
			u8 baseaddr3;
		};
	};

} s_gdt;

//GDT全局描述符
typedef struct
{
	u16 gdt_lenth;
	u16 gdt_addr;
	u16 gdt_addr2;
} s_gdtp;

//IDT全局描述符
typedef struct
{
	union
	{
		struct
		{
			u64 lidt;
		};

		struct
		{
			u32 idt, idt2;
		};

		struct
		{
			u16 offset;
			u16 selector;
			u8 bbb_no_use;
			u8 p_dpl_bbbbb;
			u16 offset2;
		};
	};

} s_idt;

//IDT全局描述符
typedef struct
{
	u16 idt_lenth;
	u16 idt_addr;
	u16 idt_addr2;
} s_idtp;

//信号量
typedef struct semaphore
{
	//信号量的值
	int value;
	//阻塞链表block
	void *list_block;
} s_sem;

//线程
typedef struct pthread_t
{
	u32 pid;
} s_pthread;

#endif
