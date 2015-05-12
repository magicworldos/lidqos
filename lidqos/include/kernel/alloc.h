/*
 * alloc.h
 * Copyright (C) Feb 10, 2014 by lidq
 *
 * 内存申请头文件
 */

#ifndef _ALLOC_H_
#define _ALLOC_H_

#include <kernel/typedef.h>

//位图状态标识
//未使用
#define MM_FREE				(0)
//已使用
#define MM_USED				(1)
//动态分配
#define MM_DYNAMIC			(2)

//内存页大小4096B
#define MM_PAGE_SIZE		(4 * 1024)
//内存总页数
#define MAP_SIZE			(0x20000)	//(MEM_SIZE * 1024 * 1024 / MM_PAGE_SIZE)
#define MAP_SIZE_LOGIC		(0x100000)	//(MEM_SIZE_LOGIC * 1024 * 1024 / MM_PAGE_SIZE)

//内核程序大小256页 256*4096 = 1MB
#define KERNEL_SIZE			(0x100)

/*
 * MMAP所在内存地址为 [0x100000, 0x200000)
 * MAP使用情况内存地址为 [0x300000, 0x700000)
 * Kernel所在页目录及页表地址 [0x700000, 0xb01000)
 * 从0x1000000以下为0x1000个内存页
 */
#define MMAP_USED_SIZE		(0x1000)

//map起始地址
#define MMAP				(0x200000)
//map_process起始地址
#define MMAP_PRO			(0x300000)

/*
 * install_alloc : 安装申请内存位图
 * return : void
 */
void install_alloc();

/*
 * alloc_page : 申请内存页，每页为4KB大小
 *  - int count : 页数
 * return : void*返回申请地址，NULL代表申请失败
 */
void* alloc_page(int count);

/*
 * alloc_page : 释放内存页，每页为4KB大小
 *  - void *addr : 释放地址
 *  - int count : 释放页数
 * return : void
 */
void free_page(void *addr, int count);

/*
 * alloc_mm : 申请内存
 *  - int size : 申请大小
 * return : void*返回申请地址，NULL代表申请失败
 */
void* alloc_mm(int size);

/*
 * free_mm : 释放内存
 *  - void* addr : 释放地址
 *  - int size : 释放大小
 * return : void
 */
void free_mm(void* addr, int size);

#endif
