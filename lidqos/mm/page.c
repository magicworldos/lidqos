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
	u16 temp;
	__asm__ volatile("movw	%%ds, %0" : "=a" (temp) : );
	printf("%x\n", temp);
	hlt();

	//test();
	//关闭分页
	__asm__ volatile(
			"movl	%cr0, %eax;"
			"andl	$0x7FFFFFFF, %eax;"
			"movl    %eax, %cr0;"
	);

	//取得页面错误地址
	u32 addr = cr2();
	//printf("%x\n", addr);

	//页目录索引 / 4M
	u32 page_dir_index = addr / 0x400000;
	//页表索引 / 4K
	u32 page_table_index = (addr % 0x400000) / 0x1000;
	//页内偏移
	u32 page_inside = (addr % 0x400000) % 0x1000;

	u32 cr3;
	__asm__ volatile("movl	%%cr3, %0" : "=a" (cr3) : );
	u32 *page_dir = (u32 *) cr3;
	u32 *page_table = NULL;
	//如果页目录项不存在
	if ((page_dir[page_dir_index] & 0x1) == 0)
	{
		//申请4096 × 2大小，前4096为页表
		//此内容不能被换出
		//后4096为交换分区扇区号
		page_table = (u32 *) alloc_page(1, MM_SWAP_TYPE_NO);
		//这个目录项的页表已存在
		page_dir[page_dir_index] = ((u32) page_table | 7);

		//页表1有1024个，每个4字节，共4096个字节
		for (int i = 0; i < MEMORY_RANGE / PAGE_SIZE; ++i)
		{
			//这些页表尚未分配
			page_table[i] = 6;
		}

		int end_flag = 0;
		for (int i = 0; i < 1024 && !end_flag; i++)
		{
			if ((page_dir[i] & 0x1) == 1)
			{
				u32 *page_tbl = (u32 *) (page_dir[i] & 0xFFFFF000);
				for (int j = 0; j < 1024 && !end_flag; j++)
				{
					if ((page_tbl[j] & 0x1) == 0 && (page_tbl[j] & 0x200) == 0)
					{
						page_tbl[j] = (u32) page_table | 7;
						end_flag = 1;
						//printf("Move OK %x %x %x\n", page_table, i, j);
					}
				}
			}
		}

	}

	//页目录索引 / 4M
	u32 p_dir_ind = (u32) page_table / 0x400000;
	//页表索引 / 4K
	u32 p_table_ind = ((u32) page_table % 0x400000) / 0x1000;

	if (!(page_dir_index == p_dir_ind && page_table_index == p_table_ind))
	{
		//取得页表
		page_table = (u32 *) (page_dir[page_dir_index] & 0xFFFFF000);
		//如果此页表不在内存中
		if ((page_table[page_table_index] & 0x1) == 0)
		{
			//尚未分配
			if ((page_table[page_table_index] & 0x200) == 0)
			{
				//申请一个空白页
				u32 alloc_addr = (u32) alloc_page(1, MM_SWAP_TYPE_CAN);
				//如果内存已无可用页面
				if (alloc_addr == 0)
				{
					//与虚拟内存交换
					//将交换后的页面地址赋值给alloc_addr
					//printf("Page E0\n");
				}
				//更新页表值
				page_table[page_table_index] = alloc_addr | 7;

				//printf("Page OK %x %x\n",page_table_index, alloc_addr);
			}
			//已分配，但被换出到交换分区中
			else if ((page_table[page_table_index] & 0x200) == 1)
			{
				//申请一个空白页
				//void *alloc_addr = NULL;
				//与虚拟内存交换
				//将交换后的页面地址赋值给alloc_addr
				//更新页表值
				//page_table[page_table_index] = (u32) alloc_addr | 7;
				//printf("Page E1\n");
			}
		}
		else
		{
			//printf("Page E2\n");
		}
	}

	__asm__ volatile(
			"movl	%cr0, %eax;"
			"orl	$0x80000000, %eax;"
			"movl    %eax, %cr0;"
	);
}
