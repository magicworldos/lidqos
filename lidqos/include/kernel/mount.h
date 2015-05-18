/*
 * mount.h
 *
 *  Created on: Feb 12, 2015
 *      Author: lidq
 */

#ifndef _INCLUDE_KERNEL_MOUNT_H_
#define _INCLUDE_KERNEL_MOUNT_H_

#include <kernel/typedef.h>
#include <kernel/tree.h>
#include <kernel/sys_var.h>
#include <kernel/osiso.h>
#include <kernel/alloc.h>
#include <kernel/string.h>

typedef struct s_mount
{
	//挂载点 mount_point
//	u32 mp_dev_id;
//	u32 mp_fs_no;

//被挂载 mounted
//	u32 md_dev_id;
//	u32 md_fs_no;

	u32 dev_id;
	char path[0x200];
} s_mount;

void mount_root();

u32 find_dev_id_fullpath(char *full_path, char *out_path);

u32 find_dev_id(s_tree *tree, char *path);

int find_mount_id_fullpath(char *full_path);

int find_mount_id(s_tree *tree, char *path);

void mount_hda(s_pt *pts, char *mount_point);

#endif /* INCLUDE_KERNEL_MOUNT_H_ */
