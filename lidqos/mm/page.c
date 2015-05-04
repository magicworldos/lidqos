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
		for (int j = 0; j < 1024; j++)
		{
			page_table[j] = address | 7;
			address += MM_PAGE_SIZE;
		}
		page_dir[i] = ((u32) page_table | 7);
		page_table += 1024;
	}

	set_cr3(PAGE_DIR);

	open_mm_page();
}

void page_error(u32 error_code)
{
	printf("Seqestion error. %x\n", error_code);

	if (error_code == 7)
	{
		hlt();
	}

	//取得页面错误地址
	u32 error_addr = cr2();
	u32 v_cr3 = cr3();

	set_cr3(PAGE_DIR);

	u32 *page_dir = (u32 *) v_cr3;

	//页目录索引 / 4M
	u32 page_dir_index = error_addr / 0x400000;
	//页表索引 / 4K
	u32 page_table_index = (error_addr % 0x400000) / 0x1000;
	//页内偏移
	u32 page_inside = (error_addr % 0x400000) % 0x1000;

	u32 *tbl = alloc_page(3, 1, MM_SWAP_TYPE_CAN);
	page_dir[page_dir_index] = (u32) tbl | 7;
	u32 address = error_addr & 0x3FFFFF;
	for (int i = 0; i < 1024; i++)
	{
		if (i == page_table_index)
		{
			tbl[i] = address | 7;
		}
		else
		{
			tbl[i] = 6;
		}
		address += 0x1000;
	}
	set_cr3(v_cr3);
}
