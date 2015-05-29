/*
 * system.h
 *
 *  Created on: Nov 26, 2014
 *      Author: lidq
 */

#ifndef _SYSTEM_H_
#define _SYSTEM_H_

#include <kernel/typedef.h>
#include <shell/stdio.h>
#include <shell/stdlib.h>
#include <shell/sys_hd.h>

int main(int argc, char **args);

void create_partition_table();

void load_partition_table();

void format_hda(u32 uid, u32 gid, u32 mode);

u32 hda_alloc(u32 dev_id);

void read_sector(u32 lba, u8* buff);

void write_sector(u32 lba, u8* buff);

void install_swap(s_pt *pts);

void mount_hda(s_pt *pts, char *mount_point);

void install_pts_to_kernel();

void install_program(char *path, char *args);

#endif /* SYSTEM_H_ */
