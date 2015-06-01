/*
 * fs.c
 * Copyright (C) Feb 10, 2014 by lidq
 *
 * 文件操作
 */

#ifndef _FS_C_
#define _FS_C_

#include <kernel/typedef.h>
#include <shell/fs.h>
#include <kernel/sys_var.h>
#include <shell/stdlib.h>

extern s_session *session;

/*
 * fopen : 打开文件
 *  - char *file_name : 完整路径文件名
 *  - int fs_mode : FS_MODE_WRITE为写模式，FS_MODE_READ为读模式
 * return : s_file*文件结构指针
 */
FILE* fopen(char *file, int mode)
{
	int params[6];

	u32 uid = session->uid;
	u32 gid = session->gid;

	FILE *fp = malloc(sizeof(FILE));
	params[0] = 0;
	params[1] = (int) file;
	params[2] = mode;
	params[3] = (int) uid;
	params[4] = (int) gid;
	params[5] = (int) fp;
	__asm__ volatile("int	$0x86" :: "a"(params));

	return fp;
}

/*
 * fclose : 关闭文件
 *  - s_file *fp : 文件指针
 * return : -1为失败， 0 为成功
 */
void fclose(FILE *fp)
{
	int params[2];
	params[0] = 1;
	params[1] = (int) fp;
	__asm__ volatile("int	$0x86" :: "a"(params));

	free(fp);
}

/*
 * fwrite : 写入数据到文件
 *  - s_file *fp : 文件指针
 *  - int size : 写入字节数
 *  - char *data : 写入数据
 * return : void
 */
void fwrite(FILE *fp, int size, char *data)
{
	int params[4];
	params[0] = 2;
	params[1] = (int) fp;
	params[2] = size;
	params[3] = (int) data;
	__asm__ volatile("int	$0x86" :: "a"(params));
}

/*
 * fread : 读取文件内容
 *  - s_file *fp : 文件指针
 *  - int size : 读入字节数
 *  - char *data : 等待读入的数据地址
 * return : void
 */
void fread(FILE *fp, int size, char *data)
{
	int params[4];
	params[0] = 3;
	params[1] = (int) fp;
	params[2] = size;
	params[3] = (int) data;
	__asm__ volatile("int	$0x86" :: "a"(params));
}

/*
 * fgetch : 读一个字符
 *  - s_file *fp : 文件指针
 * return : char读入的字符
 */
char fgetch(FILE *fp)
{
	char ch = 0;
	int params[3];
	params[0] = 4;
	params[1] = (int) fp;
	params[2] = (int) &ch;
	__asm__ volatile("int	$0x86" :: "a"(params));
	return ch;
}

/*
 * fgetline : 读一行
 *  - s_file *fp : 文件指针
 *  - char *data : 数据地址
 * return : void
 */
void fgetline(FILE *fp, char *data)
{
	int params[3];
	params[0] = 5;
	params[1] = (int) fp;
	params[2] = (int) data;
	__asm__ volatile("int	$0x86" :: "a"(params));
}

/*
 * fputch : 写一个字符
 *  - s_file *fp : 文件指针
 *  - char ch : 写入字符
 * return : void
 */
void fputch(FILE *fp, char ch)
{
	int params[3];
	params[0] = 6;
	params[1] = (int) fp;
	params[2] = (int) ch;
	__asm__ volatile("int	$0x86" :: "a"(params));
}

/*
 * fputline : 写一行
 *  - s_file *fp : 文件指针
 *  - char ch : 写入数据地址
 * return : void
 */
void fputline(FILE *fp, char *data)
{
	int params[3];
	params[0] = 7;
	params[1] = (int) fp;
	params[2] = (int) data;
	__asm__ volatile("int	$0x86" :: "a"(params));
}

/*
 * feof : 是否为文件尾
 *  - s_file *fp : 文件指针
 * return : int 0不是文件尾，1是文件尾
 */
int feof(FILE *fp)
{
	int end_of_file = 0;
	int params[3];
	params[0] = 8;
	params[1] = (int) fp;
	params[2] = (int) &end_of_file;
	__asm__ volatile("int	$0x86" :: "a"(params));
	return end_of_file;
}

/*
 * f_opendir : 打开文件夹，读取其子文件（夹）到文件指针链表中
 *  - char *path_name : 完整路径的文件夹名称
 * return : s_file*子文件（夹）链表头
 */
FILE* fopendir(char *path_name)
{
	FILE* fs = NULL;
	int params[3];
	params[0] = 9;
	params[1] = (int) path_name;
	params[2] = (int) &fs;
	__asm__ volatile("int	$0x86" :: "a"(params));
	return fs;
}

/*
 * f_closedir : 关闭文件链表
 *  - s_file* fp : 文件链表头
 * return : void
 */
void fclosedir(FILE* fp)
{
	int params[2];
	params[0] = 10;
	params[1] = (int) fp;
	__asm__ volatile("int	$0x86" :: "a"(params));
}
////////////////////////
int fs_find_path_by_user(char *path, u32 uid, u32 gid, u32 *dev_id, s_fs *fs)
{
	int status = 0;
	int params[7];
	params[0] = 0;
	params[1] = (int) path;
	params[2] = (int) uid;
	params[3] = (int) gid;
	params[4] = (int) &fs;
	params[5] = (int) dev_id;
	params[6] = (int) &status;
	__asm__ volatile("int	$0x87" :: "a"(params));
	return status;
}

int fs_create_folder(char *path, char *folder_name, u32 uid, u32 gid, u32 mode)
{
	int status;
	int params[7];
	params[0] = 3;
	params[1] = (int) path;
	params[2] = (int) folder_name;
	params[3] = (int) uid;
	params[4] = (int) gid;
	params[5] = (int) mode;
	params[6] = (int) &status;
	__asm__ volatile("int	$0x87" :: "a"(params));
	return status;
}

#endif
