/*
 * sys_hd.h
 *
 *  Created on: May 29, 2015
 *      Author: lidq
 */

#ifndef _INCLUDE_SHELL_SYS_HD_H_
#define _INCLUDE_SHELL_SYS_HD_H_

#include <kernel/typedef.h>
#include <kernel/sys_var.h>
#include <kernel/fsdef.h>


#define HD_READ         	0x20
#define HD_WRITE        	0x30

//一个扇区大小除以一个分区表项大小 512 / sizeof(s_pt) = 32
#define HD_PT_COUT			(512 / 16)

//hda的bitmap大小
//为表示1G大小，则需要0x200000个扇区
//一个字节的每一个bit位表示一个扇区，则需要0x200000/8=0x40000个字节
//40000个字节本身需要使用0x40000/0x200=0x200个扇区来存储
//则bitmap的大小为0x200
//HDA_BITMAP_SIZE+2是因为0号扇区为启动扇区，1号扇区为分区表扇区

//hda1 bitmap为0x200个扇区，可表示1GB
#define HDA1_BITMAP_START	(2)
#define HDA1_SIZE			(0x200000)
#define HDA1_START			(HDA1_BITMAP_START + (HDA1_SIZE / 8 / 0x200))

//hda2 bitmap为0x400个扇区，可表示2GB
#define HDA2_BITMAP_START	(HDA1_START + HDA1_SIZE)
#define HDA2_SIZE			(0x400000)
#define HDA2_START			(HDA2_BITMAP_START + (HDA2_SIZE / 8 / 200))

//hda3 swap交换分区 bitmap为0x100个扇区，每个bit表示8个连续扇区，共可表示4GB
#define HDA3_BITMAP_START	(HDA2_START + HDA2_SIZE)
#define HDA3_SIZE			(0x800000)
#define HDA3_START			(HDA3_BITMAP_START + 0x100)

int main(int argc, char **args);

void init_sys_var();

void create_partition_table();

void load_partition_table();

void format_hda(u32 uid, u32 gid, u32 mode);

u32 hda_alloc(u32 dev_id);

void read_sector(u32 lba, u8* buff);

void write_sector(u32 lba, u8* buff);

void install_swap(s_pt *pts);

void mount_hda(s_pt *pts, char *mount_point);

#endif /* INCLUDE_SHELL_SYS_HD_H_ */
