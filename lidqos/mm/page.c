/*
 * page.c
 *
 *  Created on: Apr 20, 2015
 *      Author: lidq
 */

#include <kernel/page.h>

void install_page()
{
	//页目录开始于0x300000，大小为1024个（每个4字节）共4096字节
	u32 *page_dir = ((u32 *) PAGE_DIR);
	//页表1开始于0x300000 + 0x1000 = 0x301000
	u32 *page_table = ((u32 *) PAGE_TABLE);
	//用于内存地址计算
	u32 address = 0;
	//处理所有页目录
	for (int i = 0; i < 1024; i++)
	{
		for (int j = 0; j < 1024; j++)
		{
			page_table[j] = address | 7;
			address += PAGE_SIZE;
		}
		page_dir[i] = ((u32) page_table | 7);
		page_table += 1024;
	}

	__asm__ volatile("movl	%%eax, %%cr3" :: "a"(PAGE_DIR));
	__asm__ volatile(
			"movl	%cr0, %eax;"
			"orl	$0x80000000, %eax;"
			"movl    %eax, %cr0;"
	);

}

void page_error()
{
	//取得页面错误地址
	u32 error_addr = cr2();
	u32 cr3 = 0;
	__asm__ volatile("movl	%%cr3, %0" : "=a" (cr3) : );

	__asm__ volatile("movl	%%eax, %%cr3" :: "a"(PAGE_DIR));

	u32 *page_dir = (u32 *) cr3;
	printf("%x\n", error_addr);

	//页目录索引 / 4M
	u32 page_dir_index = error_addr / 0x400000;
	//页表索引 / 4K
	u32 page_table_index = (error_addr % 0x400000) / 0x1000;
	//页内偏移
	u32 page_inside = (error_addr % 0x400000) % 0x1000;

	u32 *tbl = alloc_page(1, MM_SWAP_TYPE_CAN);
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

	printf("alloc missing page OK.\n");

	__asm__ volatile("movl	%%eax, %%cr3" :: "a"(cr3));
}
