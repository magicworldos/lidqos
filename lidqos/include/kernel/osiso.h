/*
 * osiso.h
 * Copyright (C) Feb 10, 2014 by lidq
 *
 * 系统光盘驱动头文件
 */

#ifndef _OSISO_H_
#define _OSISO_H_

#include <kernel/typedef.h>
#include <kernel/hd.h>
#include <kernel/io.h>
#include <kernel/mm.h>

//定义IDE设备总线
//主
#define ATA_BUS_PRIMARY     0x1F0
//从
#define ATA_BUS_SECONDARY   0x170

//主
#define ATA_DRIVE_MASTER    0xE0
//从
#define ATA_DRIVE_SLAVE     0xF0

//设备类型
//系统光盘
#define HD_DEV_ID_OSISO			(0)

#define HD_DEV_ID_HDA_MIN		(0x1)
//hda1
#define HD_DEV_ID_HDA1			(0x1)
//hda2
#define HD_DEV_ID_HDA2			(0x2)
//hda_max_no
#define HD_DEV_ID_HDA_MAX		(0x100)

#define OSISO_START_LBA			(0x400)
#define OSISO_START_SNO			(OSISO_START_LBA)
//map大小为256KB，可表示1GB容量使用情况
#define ISO_MAP_SIZE 			(256 * OSISO_START_SNO)
//扇区大小
#define SECTOR_SIZE				(512)
//有效扇区起始于512
#define SECTOR_START 			(ISO_MAP_SIZE / SECTOR_SIZE)
//map占用512个扇区
#define MAP_SECTOR_SIZE			SECTOR_START

/*
 * send_atapi_command : 向atapi设备发送命令组
 *  -  : u16 *cmd 命令组
 *  -  : u16 port 操作端口
 * return : void
 */
void send_atapi_command(u16 *cmd, u16 port);

/*
 * osiso_read_block : 读块数据
 *  - u32 bus : 总线端口
 *  - u32 drive : 设备类型
 *  - u32 lba : 线性编号
 *  - void *data : 数据地址
 * return : void
 */
void osiso_read_block(u32 bus, u32 drive, u32 lba, char *data);

void osiso_read_block_phy(u32 bus, u32 drive, u32 lba, char *data);

/*
 * osiso_write_block : 读块数据
 *  - u32 bus : 总线端口
 *  - u32 drive : 设备类型
 *  - u32 lba : 线性编号
 *  - void *data : 数据地址
 * return : void
 */
void osiso_write_block(u32 bus, u32 drive, u32 lba, void *buff);

/*
 * osiso_load_root : 读根目录的块数据
 *  - s_fs *fs_root : 根目录指针
 * return : void
 */
void osiso_load_root(s_fs *fs_root);

/*
 * osiso_alloc_sec : 申请一个空块
 *
 * return : u32空块编号
 */
u32 osiso_alloc_sec();

#endif
