/*
 * config.h
 * Copyright (C) Feb 10, 2014 by lidq
 *
 * 条件编译头文件
 */

#ifndef _CONFIG_H_
#define _CONFIG_H_

//debug
#define DEBUG							(0)

//create partition table
//format hda1 hda2
#define PT_CREATE_FORMAT				(0)
#define HD_WR_TRY_TIMES					(0x10)

//死循环
#define RUNNING_WITH_FOR				{for (;;){}}

//安装内核程序相关
//系统光盘内核起始扇区(2048B/扇区)
#define KERNEL_INS_START_SECTOR			(297)
//临时载入内存地址
#define KERNEL_INS_TEMP_ADDR			(0x1000000)
//临时载入内存地址（从0x10000开始不能覆盖.data段和当前内核.text内容）
#define KERNEL_INS_TEMP_ADDR_STR		"$0x1004000"
//需要安装的内核程序大小0x200000字节
#define KERNEL_INS_SECTOR_COUNT			(256)
#define KERNEL_INS_ADDR_STR				"$0x4000"
#define KERNEL_INS_SIZE_STR				"$0x84000"

#endif
