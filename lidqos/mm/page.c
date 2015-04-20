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
	u32 *page_table1 = ((u32 *) PAGE_TABLE);
	//页表2开始于0x301000 + 0x1000 = 0x302000
	u32 *page_table2 = ((u32 *) (PAGE_TABLE + PAGE_SIZE));
	u32 address = 0;
	//页表1有1024个，每个4字节，共4096个字节
	for (int i = 0; i < MEMORY_RANGE / PAGE_SIZE; ++i)
	{
		page_table1[i] = address | 7;
		address += PAGE_SIZE;

	};
	//页表2有1024个，每个4字节，共4096个字节
	for (int i = 0; i < MEMORY_RANGE / PAGE_SIZE; ++i)
	{
		page_table2[i] = address | 7;
		address += PAGE_SIZE;
	};

	page_dir[0] = ((u32) page_table1 | 7);
	page_dir[1] = ((u32) page_table2 | 7);

	for (int i = 2; i < 1024; ++i)
	{
		page_dir[i] = 6;
	}

	__asm__ volatile("movl	%%eax, %%cr3" :: "a"(PAGE_DIR));

	__asm__ volatile(
			"movl	$0x80000000, %eax;"
			"movl    %eax, %cr0;"
	);
}
