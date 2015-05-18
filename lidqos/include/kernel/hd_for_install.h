/*
 * hd.h
 * Copyright (C) Feb 10, 2014 by lidq
 *
 * 块设备驱动头文件
 */

#ifndef _HD_H_
#define _HD_H_

#include <kernel/typedef.h>
#include <kernel/osiso.h>
#include <kernel/fs_for_install.h>

/*
 * read_block : 读块数据
 *  - u32 hd_type : 块设备类型
 *  - u32 lba : 线性编号
 *  - void *data : 数据地址
 * return : void
 */
void read_block(u32 hd_type, u32 lba, void *data);

/*
 * write_block : 写块数据
 *  - u32 hd_type : 块设备类型
 *  - u32 lba : 线性编号
 *  - void *data : 数据地址
 * return : void
 */
void write_block(u32 hd_type, u32 lba, void *data);

/*
 * load_root : 载入根文件块内容
 *  - u32 hd_type : 块设备类型
 *  - void *fs_root : 数据地址
 * return : void
 */
void load_root(u32 hd_type, void *fs_root);

/*
 * alloc_sec : 申请一个空的块
 *  - u32 hd_dev_id : 块设备类型
 * return : u32块编号
 */
u32 alloc_sec(u32 hd_dev_id);

#endif
