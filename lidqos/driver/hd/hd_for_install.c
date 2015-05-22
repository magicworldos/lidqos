/*
 * hd.c
 * Copyright (C) Feb 10, 2014 by lidq
 *
 * 块设备操作驱动:
 *  - 磁盘、光盘等盲区块设备的读写驱动程序
 */

#include <kernel/hd_for_install.h>
#include <kernel/sys_var.h>

/*
 * read_block : 读块数据
 *  - u32 hd_dev_id : 块设备类型
 *  - u32 lba : 线性编号
 *  - void *data : 数据地址
 * return : void
 */
void read_block(u32 hd_dev_id, u32 lba, void *data)
{
	osiso_read_block(ATA_BUS_PRIMARY, ATA_DRIVE_SLAVE, lba, (char *) data);
}

/*
 * write_block : 写块数据
 *  - u32 hd_dev_id : 块设备类型
 *  - u32 lba : 线性编号
 *  - void *data : 数据地址
 * return : void
 */
void write_block(u32 hd_dev_id, u32 lba, void *data)
{
	osiso_write_block(ATA_BUS_PRIMARY, ATA_DRIVE_SLAVE, lba, (char *) data);
}

/*
 * load_root : 载入根文件块内容
 *  - u32 hd_dev_id : 块设备类型
 *  - void *fs_root : 数据地址
 * return : void
 */
void load_root(u32 hd_dev_id, void *fs_root)
{
	osiso_load_root(fs_root);
}

/*
 * alloc_sec : 申请一个空的块
 *  - u32 hd_dev_id : 块设备类型
 * return : u32块编号
 */
u32 alloc_sec(u32 hd_dev_id)
{
	return osiso_alloc_sec();
}
