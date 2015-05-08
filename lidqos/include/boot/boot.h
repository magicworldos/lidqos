/*
 * boot.h
 * Copyright (C) Feb 10, 2014 by lidq
 *
 * 启动程序头文件:
 *  - 定义了启动程序所需要的相关宏
 */

#ifndef _BOOT_H_
#define _BOOT_H_

//定义启动段地址为0x7c00
#define	_SEG_BOOT 0x7c0

//定义启动程序大小0x800 * 0x4 = 0x2000
#define	_SEG_BOOT_SIZE 0x2000

//定义内核程序所在位置0x9c00 (_SEG_BOOT + _SEG_BOOT_SIZE)
#define _SEG_KERNEL 0x9c0
#define _SEG_KERNEL_PH 0x9c00

//定义启动main函数段地址为0x90000
#define _SEG_MAIN 0x9000

//定义内核程序.Ttext的offset
#define _SEG_KERNEL_OFFSET 0x2000
#define _SEG_KERNEL_DATA_OFFSET 0x1000000

//内核大小256个扇区*2048
#define _KERNEL_SIZE 0x80000

//定义内核地址为0的全局描述符的选择子
#define _GDT_IND_KERNEL 0x10
#define _GDT_IND_KERNEL_DATA 0x18

#endif
