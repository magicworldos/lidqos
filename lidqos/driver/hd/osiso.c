/*
 * osiso.c
 * Copyright (C) Feb 10, 2014 by lidq
 *
 * 系统光盘驱动:
 *  - 主要处理系统光盘的读写操作
 */

#ifndef _OSISO_C_
#define _OSISO_C_

#include <kernel/osiso.h>

/*
 * send_atapi_command : 向atapi设备发送命令组
 *  -  : u16 *cmd 命令组
 *  -  : u16 port 操作端口
 * return : void
 */
void send_atapi_command(u16 *cmd, u16 port)
{
	//循环发送6个命令
	for (int i = 0; i < 6; i++)
	{
		outw_p(cmd[i], port);
	}
}

/*
 * osiso_read_block : 读块数据
 *  - u32 bus : 总线端口
 *  - u32 drive : 设备类型
 *  - u32 lba : 线性编号
 *  - void *data : 数据地址
 * return : void
 */
void osiso_read_block(u32 bus, u32 drive, u32 lba, char *data)
{
	//osiso起始扇区号为0x400 = 1024
	u8 block_no = lba % 4;
	lba = OSISO_START_LBA + (lba / 4);

	char buff[ATAPI_SECTOR_SIZE];
	u8 read_status = 0;
	do
	{
		//0xA8为读取扇区数据的命令
		u8 read_cmd[12] =
		{ 0xA8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
		//设置设备号为IDE0从设备
		outb_p(drive & (1 << 4), ATA_DRIVE_SELECT(bus));
		outb_p(ATAPI_SECTOR_SIZE & 0xFF, ATA_ADDRESS2(bus));
		outb_p(ATAPI_SECTOR_SIZE >> 8, ATA_ADDRESS3(bus));
		outb_p(0xa0, ATA_COMMAND(bus));
		//等待设备空闲
		while ((read_status = inb_p(ATA_STATUS(bus))) & 0x80)
		{
		}
		while (!((read_status = inb_p(ATA_STATUS(bus))) & 0x8) && !(read_status & 0x1))
		{

		}
		//设备异常
		if (read_status & 0x1)
		{
			return;
		}
		//读取1个扇区
		read_cmd[9] = 1;
		//设置扇区号
		read_cmd[2] = (lba >> 0x18) & 0xFF;
		read_cmd[3] = (lba >> 0x10) & 0xFF;
		read_cmd[4] = (lba >> 0x08) & 0xFF;
		read_cmd[5] = (lba >> 0x00) & 0xFF;
		//发送命令
		send_atapi_command((u16 *) read_cmd, ATA_DATA(bus));
		//等待设备就绪
		while (inb_p(ATA_STATUS(bus)) < 0)
		{
		}
		while ((inb_p(ATA_STATUS(bus)) & 0xc0) != 0x40)
		{
		}
		//读取数据
		insl_p((u32)buff, ATA_DATA(bus), 1);
		//读取操作状态
		read_status = inb_p(ATA_STATUS(bus));
		if (read_status != 0x50)
		{
		}
		u8 *f = (u8 *) buff + 0x200 * block_no;
		u8 *t = (u8 *) data;
		for (u32 i = 0; i < 0x200; i++)
		{
			*(t + i) = *(f + i);
		}

	}
	while (read_status != 0x50);
}

void osiso_read_block_phy(u32 bus, u32 drive, u32 lba, char *data)
{
	u8 read_status = 0;
	do
	{
		/* 0xA8 is READ SECTORS command byte. */
		u8 read_cmd[12] =
		{ 0xA8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

		outb_p(drive & (1 << 4), ATA_DRIVE_SELECT(bus));

		outb_p(ATAPI_SECTOR_SIZE & 0xFF, ATA_ADDRESS2(bus));
		outb_p(ATAPI_SECTOR_SIZE >> 8, ATA_ADDRESS3(bus));

		outb_p(0xa0, ATA_COMMAND(bus)); /* ATA PACKET command */

		while ((read_status = inb_p(ATA_STATUS(bus))) & 0x80) /* BUSY */
		{

		}
		while (!((read_status = inb_p(ATA_STATUS(bus))) & 0x8) && !(read_status & 0x1))
		{

		}
		/* DRQ or ERROR set */
		if (read_status & 0x1)
		{
			return;
		}
		read_cmd[9] = 1; /* 1 sector */
		read_cmd[2] = (lba >> 0x18) & 0xFF; /* most sig. byte of LBA */
		read_cmd[3] = (lba >> 0x10) & 0xFF;
		read_cmd[4] = (lba >> 0x08) & 0xFF;
		read_cmd[5] = (lba >> 0x00) & 0xFF; /* least sig. byte of LBA */

		/* Send ATAPI/SCSI command */
		send_atapi_command((u16 *) read_cmd, ATA_DATA(bus));

		/* Wait for IRQ that says the data is ready. */
		//schedule();
		while (inb_p(ATA_STATUS(bus)) < 0)
		{
		}

		while ((inb_p(ATA_STATUS(bus)) & 0xc0) != 0x40)
		{
		}

		insl_p((u32)data, ATA_DATA(bus), 1);

		read_status = inb_p(ATA_STATUS(bus));
		if (read_status != 0x50)
		{
		}
	}
	while (read_status != 0x50);
}

/*
 * osiso_write_block : 读块数据
 *  - u32 bus : 总线端口
 *  - u32 drive : 设备类型
 *  - u32 lba : 线性编号
 *  - void *data : 数据地址
 * return : void
 */
void osiso_write_block(u32 bus, u32 drive, u32 lba, void *buff)
{
	//光盘不能写
}

/*
 * osiso_load_root : 读根目录的块数据
 *  - s_fs *fs_root : 根目录指针
 * return : void
 */
void osiso_load_root(s_fs *fs_root)
{
	//读根目录的块数据
	osiso_read_block(ATA_BUS_PRIMARY, ATA_DRIVE_SLAVE, SECTOR_START, (char *) fs_root);
}

/*
 * osiso_alloc_sec : 申请一个空块
 *
 * return : u32空块编号
 */
u32 osiso_alloc_sec()
{
	//光盘不能写，所以返回0
	return 0;
}

#endif
