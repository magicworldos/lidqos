/*
 * swap.c
 *
 *  Created on: May 16, 2015
 *      Author: lidq
 */

#include <kernel/swap.h>

s_pt *swap_pts = NULL;

/*
 * 安装交换分区，此时尚未真正的处理磁盘设备
 * 这里只是使用了一个4G大小的物理磁盘
 * 将4G的物理磁盘作为swap交换分区
 * 因为页表中只有高20位可以存放地址数据，也就是为什么在分页时要对内存做4K对齐
 * 当页面被换出到交换分区之后，页表中的高20位转变为存放20位的磁盘扇区号
 * 系统启动时，初始化交换分区，将交换分区的map初始化为0
 */
void install_swap(s_pt *pts)
{
	swap_pts = pts;

	u8 bitmap[0x200];
	for (int i = 0; i < 0x200; i++)
	{
		bitmap[i] = 0;
	}
	//map大小为： 4G / 4096 / 8 = 0x20000
	u32 bitmap_size = 0x20000;
	for (u32 j = 0; j < bitmap_size / 0x200; j++)
	{
		hd_rw(swap_pts->bitmap_start + j, HD_WRITE, bitmap);
	}
}

/*
 * 向交换分区申请一个可用的磁盘号
 */
u32 swap_alloc_sec()
{
	if (swap_pts == NULL)
	{
		printf(0, "swap error\n");
		return 0xffffffff;
	}

	u8 bitmap[0x200];
	//map大小为： 4G / 4096 / 8 = 0x20000
	u32 bitmap_size = 0x20000;
	for (u32 j = 0; j < bitmap_size / 0x200; j++)
	{
		hd_rw(swap_pts->bitmap_start + j, HD_READ, bitmap);
		for (u32 k = 0; k < 0x200; k++)
		{
			for (u32 l = 0; l < 8; l++)
			{
				if (((bitmap[k] >> l) & 0x1) == 0)
				{
					bitmap[k] |= (0x1 << l);
					hd_rw(swap_pts->bitmap_start + j, HD_WRITE, bitmap);
					u32 sec_no = (j * 0x200 * 8) + k * 8 + l;
					return sec_no;
				}
			}
		}
	}
	return 0xffffffff;
}

/*
 * 向交换分区释放一个磁盘号
 */
void swap_free_sec(u32 sec_no)
{
	u8 bitmap[0x200];
	u32 ind = sec_no / 8 / 0x200;
	hd_rw(swap_pts->bitmap_start + ind, HD_READ, bitmap);
	bitmap[(sec_no % (0x200 * 8)) / 8] &= ~(0x1 << ((sec_no % (0x200 * 8)) % 8));
	hd_rw(swap_pts->bitmap_start + ind, HD_WRITE, bitmap);
}

/*
 * 将内存数据写入交换分区
 */
void swap_write_page(u32 sec_no, void *page_data)
{
	//计算物理扇区号
	sec_no *= 8;
	//跳过map所在扇区
	sec_no += swap_pts->start;

	//写入8个扇区
	for (int i = 0; i < 8; i++)
	{
		hd_rw(sec_no + i, HD_WRITE, page_data);
		page_data += 0x200;
	}
}

/*
 * 从交换分区读入数据到内存
 */
void swap_read_page(u32 sec_no, void *page_data)
{
	//计算物理扇区号
	sec_no *= 8;
	//跳过map所在扇区
	sec_no += swap_pts->start;

	//读出8个扇区
	for (int i = 0; i < 8; i++)
	{
		hd_rw(sec_no + i, HD_READ, page_data);
		page_data += 0x200;
	}
}
