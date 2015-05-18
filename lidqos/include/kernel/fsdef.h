/*
 * fs.h.c
 * Copyright (C) Feb 10, 2014 by lidq
 *
 * 文件定义头文件
 */

#ifndef _FSDEF_H_
#define _FSDEF_H_

#include <kernel/typedef.h>

//根目录名称
#define ROOT_NAME 				"/"

//系统光盘根目录起始扇区号
//#define OSISO_ROOT_SNO		(4096 + 512)

//数据块可存储字节数
#define FS_DATA_COUNT			(0x200)
#define FS_READ_BUFF			(0x2000)
#define FS_READ_BUFF_BC			(FS_READ_BUFF / FS_DATA_COUNT)

//数据块可存储地址数
#define FS_ADDR_COUNT			(0x80)

//文件夹类型
#define FS_FOLDER_MODE 			(01755)
//文件类型
#define FS_FILE_MODE 			(0755)

//文件名长度
#define FS_NAME_LENGTH			(0x80)

//文件写模式
#define FS_MODE_WRITE			(0x0)
//文件读模式
#define FS_MODE_READ			(0x1)

#define FS_STATUS_OK			(0)
#define FS_STATUS_NO_FILE_DIR	(1)
#define FS_STATUS_NO_PERMISSION	(2)

//文件块数据结构
typedef struct fs_s
{
	union
	{
		//文件信息块
		struct
		{
			//模式
			u32 mode;
			//所属者
			u32 owner;
			//所属组
			u32 group;
			//大小
			u32 size;
			//./
			u32 dot;
			//../
			u32 dotdot;
			//root_no
			u32 root;
			//is root
			u32 is_root;
			//文件名
			char name[476];
			//文件地址项
			u32 address;
		};
		//一级、二级、三级地址项
		struct
		{
			u32 addr_no[FS_ADDR_COUNT];
		};
		//文件数据内容
		struct
		{
			u8 data[FS_DATA_COUNT];
		};
	};
} s_fs;

//文件数据结构
typedef struct file_s
{
	u32 dev_id;
	//磁盘文件
	struct fs_s fs;
	//文件模式r/w
	int fs_mode;
	//当前字节偏移量
	int offset;
	//当前字节偏移量
	int offset_buff;
	int offset_read_buff;
	//块内依稀量
	int block_offset;
	//逻辑块索引
	int addr_index;
	//逻辑块一级索引
	int first_index;
	//逻辑块二级索引
	int sec_index;
	//逻辑块三级索引
	int third_index;
	//磁盘数据缓存
	char *buff;

	//打开文件夹的文件链表指针
	struct file_s* next;
} s_file;

#endif /* FSDEF_H_ */
