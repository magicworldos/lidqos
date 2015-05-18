/*
 * hd.h
 * Copyright (C) Feb 10, 2014 by lidq
 *
 * 块设备驱动头文件
 */

#ifndef _HD_H_
#define _HD_H_

#include <kernel/typedef.h>
#include <kernel/fs_for_install.h>
#include <kernel/osiso.h>
#include <kernel/sys_var.h>

#define HD_PORT_DATA            0x1f0
#define HD_PORT_ERROR           0x1f1
#define HD_PORT_SECT_COUNT      0x1f2
#define HD_PORT_LBA0	        0x1f3
#define HD_PORT_LBA1			0x1f4
#define HD_PORT_LBA2	        0x1f5
#define HD_PORT_LBA3	        0x1f6
#define HD_PORT_STATUS          0x1f7
#define HD_PORT_COMMAND         0x1f7

#define HD_READ         		0x20
#define HD_WRITE        		0x30

//光盘默认扇区大小为2048字节
#define ATAPI_SECTOR_SIZE 	(2048)

//I/O设备端口，从BUS(x)宏开始作为索引使用
//数据
#define ATA_DATA(x)         (x)
//
#define ATA_FEATURES(x)     (x+1)
//操作扇区数
#define ATA_SECTOR_COUNT(x) (x+2)
//线性扇区编号1
#define ATA_ADDRESS1(x)     (x+3)
//线性扇区编号2
#define ATA_ADDRESS2(x)     (x+4)
//线性扇区编号3
#define ATA_ADDRESS3(x)     (x+5)
//设备类型IDE主从
#define ATA_DRIVE_SELECT(x) (x+6)
//命令
#define ATA_COMMAND(x)      (x+7)
//状态
#define ATA_STATUS(x)       (ATA_COMMAND(x))
//设备控制寄存器
#define ATA_DCR(x)          (x+0x206)

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
 *  - u32 hd_type : 块设备类型
 * return : u32块编号
 */
u32 alloc_sec(u32 hd_type);

void hd_rw_cmd(u32 lba, u8 com, u8* buff);

void hd_rw_data(u8 com, u8* buff);

void hd_rw_insert_queue(s_hda_info *hda_info, u32 lba, u8 com, u8 *buff, int *status);

void hd_rw_execute(s_hda_info *hda_info);

void hd_rw_finished(s_hda_info *hda_info);

void hd_rw(u32 lba, u8 com, void *buf);

#endif
