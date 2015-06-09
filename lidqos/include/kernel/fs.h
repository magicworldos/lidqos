/*
 * fs.h.c
 * Copyright (C) Feb 10, 2014 by lidq
 *
 * 文件系统头文件
 */

#ifndef _FS_H_
#define _FS_H_

#include <kernel/typedef.h>
#include <kernel/fsdef.h>
#include <kernel/mount.h>
#include <kernel/string.h>

void install_fs_system();

/*
 * fs_empty_data : 清空fs块的内容
 *  - void *fs : fs文件块
 * return : void
 */
void fs_empty_data(void *fs);

int fs_find_path_by_user(char *path_name, u32 uid, u32 gid, u32 *ret_dev_id, s_fs **fs);

int fs_find_path(char *path_name, u32 *ret_dev_id, s_fs **fs);

void fs_create_fs_address(s_fs *fs_parent);

void fs_find_sno(u32 dev_id, u32 s_no, s_fs **fs);

/*
 * fs_find_sub_fs : 根据父级文件块编号和子文件夹名称查找其编号
 *  - u32 parent_no : 父级文件块编号
 *  - char *folder_name : 子文件夹名称
 * return : u32 子文件夹编号，0代表查找失败
 */
u32 fs_find_sub_fs(u32 dev_id, u32 parent_no, char *folder_name);

///*
// * fs_create_path : 根据路径创建多级文件目录
// *  - char *path_name : 待创建的路径
// * return : void
// */
//void fs_create_path(char *path_name);
//
///*
// * fs_create_fs : 根据父级文件编号创建子文件（夹）
// *  - u32 parent_no : 低级文件块编号
// *  - char *fs_name : 待创建的文件（夹）名称
// *  - u32 fs_mode : FS_FOLDER_MODE为文件夹，FS_FILE_MODE为文件
// * return : u32 返回新创建的子文件（夹—）编号，0代表创建失败
// */
//u32 fs_create_fs(u32 dev_id, u32 parent_no, char *fs_name, u32 fs_mode);

u32 fs_create_fs_path(char *path, char *fs_name, u32 uid, u32 gid, u32 mode);

void fs_del_fs_no(u32 dev_id, u32 p_no, u32 del_no);

int fs_del_fs_path(char *path, u32 uid, u32 gid);

///*
// * fs_create_file : 创建文件
// *  - char *file_name : 文件名
// * return : int -1代表失败
// */
//int fs_create_file(char *file_name);
//
///*
// * f_create : 创建文件
// *  - char *file_name : 文件名
// * return : int -1代表失败
// */
//int f_create(char *file_name);

/*
 * f_open : 打开文件
 *  - char *file_name : 完整路径文件名
 *  - int fs_mode : FS_MODE_WRITE为写模式，FS_MODE_READ为读模式
 * return : s_file*文件结构指针
 */
s_file* f_open(char *file_name, int fs_mode, u32 uid, u32 gid);

/*
 * f_write_block : 按块写入数据到文件
 *  - s_file *fp : 文件指针
 * return : int写入字节数
 */
int f_write_block(s_file *fp);

/*
 * f_write : 写入数据到文件
 *  - s_file *fp : 文件指针
 *  - int size : 写入字节数
 *  - char *data : 写入数据
 * return : int文件当前偏移
 */
int f_write(s_file *fp, int size, char *data);

/*
 * f_read_block : 按块读入文件
 *  - s_file *fp : 文件指针
 * return : int 读入字节数
 */
int f_read_block(s_file *fp, int block_ind);

/*
 * f_is_eof : 判断是否已经是文件结尾
 *  - s_file *fp : 文件指针
 * return : 0未到结尾，1已到结尾
 */
int f_is_eof(s_file *fp);

void f_read_buff(s_file *fp);

/*
 * f_read : 读取文件内容
 *  - s_file *fp : 文件指针
 *  - int size : 读入字节数
 *  - char *data : 等待读入的数据地址
 * return : int读入字节数
 */
int f_read(s_file *fp, int size, char *data);

/*
 * f_close : 关闭文件
 *  - s_file *fp : 文件指针
 * return : -1为失败， 0 为成功
 */
int f_close(s_file *fp);

/*
 * f_opendir : 打开文件夹，读取其子文件（夹）到文件指针链表中
 *  - char *path_name : 完整路径的文件夹名称
 * return : s_file*子文件（夹）链表头
 */
s_file* f_opendir(char *path_name);

/*
 * f_closedir : 关闭文件链表
 *  - s_file* fp : 文件链表头
 * return : void
 */
void f_closedir(s_file* fp);

/*
 * fs_create_fs : 根据父级文件编号创建子文件（夹）
 *  - u32 parent_no : 低级文件块编号
 *  - char *fs_name : 待创建的文件（夹）名称
 *  - u32 fs_mode : FS_FOLDER_MODE为文件夹，FS_FILE_MODE为文件
 * return : u32 返回新创建的子文件（夹—）编号，0代表创建失败
 */
u32 fs_create_fs(u32 dev_id, u32 parent_no, char *fs_name, u32 uid, u32 gid, u32 mode);

/*
 * fs_create_file : 创建文件
 *  - char *file_name : 文件名
 * return : int -1代表失败
 */
int fs_create_file(char *file_name, u32 uid, u32 gid, u32 mode);

#endif
