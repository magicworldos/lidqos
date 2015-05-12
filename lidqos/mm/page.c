/*
 * page.c
 *
 *  Created on: Apr 20, 2015
 *      Author: lidq
 */

#include <kernel/page.h>

void install_page()
{
	//页目录开始于 [0x700000, 0x701000) ，大小为1024个（每个4字节）共4096字节
	u32 *page_dir = ((u32 *) PAGE_DIR);
	//页表1开始于0x700000 + 0x1000 = 0x701000
	u32 *page_table = ((u32 *) PAGE_TABLE);
	//用于内存地址计算
	u32 address = 0;
	//处理所有页目录
	//页表开始于 [0x701000, 0xb01000) ，大小为4M
	for (int i = 0; i < 1024; i++)
	{
		if (i < 8)
		{
			for (int j = 0; j < 1024; j++)
			{
				page_table[j] = address | 7;
				address += MM_PAGE_SIZE;
			}
			page_dir[i] = ((u32) page_table | 7);
			page_table += 1024;
		}
		else
		{
			page_dir[i] = (6);
		}
	}

	set_cr3(PAGE_DIR);

	open_mm_page();
}
