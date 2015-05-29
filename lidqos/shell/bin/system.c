/*
 * system.c
 *
 *  Created on: Nov 26, 2014
 *      Author: lidq
 */

#include <shell/system.h>

s_sys_var *sys_var = NULL;

#define PT_CREATE_FORMAT		(0)

int main(int argc, char **args)
{
	printf("Start System Process.\n");

	sys_var = malloc(sizeof(s_sys_var));

#if PT_CREATE_FORMAT
	create_partition_table();
#endif

	//载入分区表
	load_partition_table();

	//安装分区表内容到内核
	install_pts();

#if PT_CREATE_FORMAT
	format_hda(1, 1, 01755);
#endif

	//mount hda1
	mount_hda(&(sys_var->pts[0]), "/data/work/");
	printf("[ OK ] Mount hda1 to /data/work/.\n");

	//mount hda1
	mount_hda(&(sys_var->pts[1]), "/data/tool/");
	printf("[ OK ] Mount hda2 to /data/tool/.\n");

	//install hda3
	install_swap(&(sys_var->pts[2]));
	printf("[ OK ] Mount had3 to swap.\n");

	for (;;)
	{
	}

	return 0;
}

void create_partition_table()
{
	//hda1 data type == 0x00
	sys_var->pts[0].device = 0x01;
	sys_var->pts[0].boot = 0x01;
	sys_var->pts[0].id = 0x01;
	sys_var->pts[0].type = 0x00;
	sys_var->pts[0].bitmap_start = HDA1_BITMAP_START;
	sys_var->pts[0].start = HDA1_START;
	sys_var->pts[0].size = HDA1_SIZE;

	//hda2 data type == 0x00
	sys_var->pts[1].device = 0x02;
	sys_var->pts[1].boot = 0x00;
	sys_var->pts[1].id = 0x02;
	sys_var->pts[1].type = 0x00;
	sys_var->pts[1].bitmap_start = HDA2_BITMAP_START;
	sys_var->pts[1].start = HDA2_START;
	sys_var->pts[1].size = HDA2_SIZE;

	//hda3 swap type == 0x1
	sys_var->pts[2].device = 0x03;
	sys_var->pts[2].boot = 0x00;
	sys_var->pts[2].id = 0x03;
	sys_var->pts[2].type = 0x1;
	sys_var->pts[2].bitmap_start = HDA3_BITMAP_START;
	sys_var->pts[2].start = HDA3_START;
	sys_var->pts[2].size = HDA3_SIZE;

	write_sector(1, (u8*) sys_var->pts);

	printf("[ OK ] Create hda partition table.\n");
}

void load_partition_table()
{
	read_sector(1, (u8*) sys_var->pts);
	printf("[ OK ] Load hda partition table.\n");
	printf("\tDevice Boot Id Type Start End Sectors\n");
	sys_var->pt_count = 0;
	for (int i = 0; i < HD_PT_COUT; i++)
	{
		if (sys_var->pts[i].device == 0)
		{
			break;
		}
		sys_var->pt_count++;
		printf("\thda%d   %d    %d  %d    %x %x %x\n", sys_var->pts[i].device, sys_var->pts[i].boot, sys_var->pts[i].id, sys_var->pts[i].type, sys_var->pts[i].start, sys_var->pts[i].start + sys_var->pts[i].size - 1, sys_var->pts[i].size);
	}
}

void format_hda(u32 uid, u32 gid, u32 mode)
{
	u8* empty = malloc(0x200);
	for (int i = 0; i < 0x200; i++)
	{
		empty[i] = 0;
	}

	for (int i = 0; i < sys_var->pt_count; i++)
	{
		if (sys_var->pts[i].type != 0x1)
		{
			u32 bitmap_size = sys_var->pts[i].size / 8 / 0x200;
			for (int j = 0; j < bitmap_size / 0x200; j++)
			{
				write_sector(sys_var->pts[i].bitmap_start + j, empty);
			}

			s_fs fs_root;
			u8 *p = (u8*) &fs_root;
			for (int i = 0; i < 0x200; i++)
			{
				p[i] = 0;
			}
			fs_root.owner = uid;
			fs_root.group = gid;
			fs_root.mode = mode;
			fs_root.dot = hda_alloc(sys_var->pts[i].device);
			fs_root.dotdot = fs_root.dot;
			str_copy("/", fs_root.name);

			write_sector(fs_root.dot, (u8 *) &fs_root);

			printf("[ OK ] Format hda%d start: %d size: %d sectors.\n", sys_var->pts[i].device, sys_var->pts[i].start, sys_var->pts[i].size);
		}
	}
	free(empty);
}

u32 hda_alloc(u32 dev_id)
{
	u8 bitmap[0x200];
	for (int i = 0; i < sys_var->pt_count; i++)
	{
		if (sys_var->pts[i].device == dev_id)
		{
			u32 bitmap_size = sys_var->pts[i].size / 8 / 0x200;
			for (int j = 0; j < bitmap_size / 0x200; j++)
			{
				read_sector(sys_var->pts[i].bitmap_start + j, bitmap);
				for (int k = 0; k < 0x200; k++)
				{
					for (int l = 0; l < 8; l++)
					{
						if (((bitmap[k] >> l) & 0x1) == 0)
						{
							bitmap[k] |= (0x1 << l);
							write_sector(sys_var->pts[i].bitmap_start + j, bitmap);
							return k * 8 + l + sys_var->pts[i].start;
						}
					}
				}
			}
			break;
		}
	}
	return 0;
}

void read_sector(u32 lba, u8* buff)
{
	int params[4];
	params[0] = 0;
	params[1] = lba;
	params[2] = HD_READ;
	params[3] = (int) buff;
	__asm__ volatile("int $0x84" :: "a"(params));
}

void write_sector(u32 lba, u8* buff)
{
	int params[4];
	params[0] = 0;
	params[1] = lba;
	params[2] = HD_WRITE;
	params[3] = (int) buff;
	__asm__ volatile("int $0x84" :: "a"(params));
}

void install_swap(s_pt *pts)
{
	int params[2];
	params[0] = 0;
	params[1] = (int) pts;
	__asm__ volatile("int $0x85" :: "a"(params));
}

void mount_hda(s_pt *pts, char *mount_point)
{
	int params[3];
	params[0] = 1;
	params[1] = (int) pts;
	params[2] = (int) mount_point;
	__asm__ volatile("int $0x85" :: "a"(params));
}

void install_pts()
{
	int params[2];
	params[0] = 2;
	params[1] = (int) sys_var->pts;
	__asm__ volatile("int $0x85" :: "a"(params));
}

void install_program(char *path, char *args)
{
	int params[3];
	params[0] = 0;
	params[1] = (int) path;
	params[2] = (int) args;
	__asm__ volatile("int $0x80" :: "a"(params));
}
