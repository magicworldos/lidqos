#ifndef _FS_H_
#define _FS_H_

#include <kernel/typedef.h>
#include <kernel/fsdef.h>

#define eof						(0)

typedef s_file FILE;

//打开文件
FILE* fopen(char *file, int mode);

//关闭文件
void fclose(FILE *fp);

//写文件
void fwrite(FILE *fp, int size, char *data);

//读文件
void fread(FILE *fp, int size, char *data);

//读一个字符
char fgetch(FILE *fp);

//读一行
void fgetline(FILE *fp, char *data);

//写一个字符
void fputch(FILE *fp, char);

//写一行
void fputline(FILE *fp, char *data);

//文件尾
int feof(FILE *fp);

/*
 * f_opendir : 打开文件夹，读取其子文件（夹）到文件指针链表中
 *  - char *path_name : 完整路径的文件夹名称
 * return : s_file*子文件（夹）链表头
 */
FILE* fopendir(char *path_name);

/*
 * f_closedir : 关闭文件链表
 *  - s_file* fp : 文件链表头
 * return : void
 */
void fclosedir(FILE* fp);

//////////////////////////////////////////////////////////

int fs_find_path_by_user(char *path, u32 uid, u32 gid, u32 *dev_id, s_fs *fs);

int fs_create_folder(char *path, char *folder_name, u32 uid, u32 gid, u32 mode);

#endif
