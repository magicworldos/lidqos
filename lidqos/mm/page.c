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

void page_error(u32 pid, u32 error_code)
{
	//取得页面错误地址
	u32 error_addr = cr2();
	u32 v_cr3 = cr3();

	set_cr3(PAGE_DIR);

	if (error_code == 7)
	{
		printf("Segmentation fault.\n");
		hlt();
	}

	u32 *page_dir = (u32 *) v_cr3;

	//页目录索引
	u32 page_dir_index = (error_addr >> 22) & 0x3ff;
	//页表索引
	u32 page_table_index = (error_addr >> 12) & 0x3ff;
	//页号
	u32 page_no = error_addr / 0x1000;

	u32 *tbl = NULL;
	//如果页表尚未分配则申请一个页表
	if ((page_dir[page_dir_index] & 0x1) == 0)
	{
		tbl = alloc_page(pid, 1, 0);
		//处理新分配的页表所表示的页面均不在内存中
		for (int i = 0; i < 1024; i++)
		{
			tbl[i] = 6;
		}
		page_dir[page_dir_index] = (u32) tbl | 7;
	}
	//如果页表已分配
	else
	{
		//取得页表地址
		tbl = (u32 *) (page_dir[page_dir_index] & 0xfffffc00);
	}

	//使地址4k对齐
	u32 address = error_addr & 0xfffff000;

	//如果此页面并没有被换出swap状态为0
	if ((tbl[page_table_index] >> 8 & 0x1) == 0)
	{
		//如果缺页申请成功
		if (alloc_page_no(page_no))
		{
			tbl[page_table_index] = address | 7;
		}
		else
		{
			printf("Segmentation fault.\n");
			hlt();
		}
	}
	//如果此页面被已经换出则要从外存换回此页面到内存
	else
	{
		/*
		 * 如果换回成功，在执行page_swap_in时，
		 * tbl[page_table_index]的前20位存放的是此页被换出到外存的逻辑扇区号sec_no
		 * 在读取外存扇区时sec_no要乘以4，因为是4k对齐
		 */
		if (page_swap_in(page_no, tbl[page_table_index] >> 12))
		{
			/*
			 * 这里的一条赋值语句有以下3个作用：
			 * 1.修改页表前20位为线性地址
			 * 2.修改swap位为0，表示没有被换入
			 * 3.修改后3位为在内存中，并可读写
			 */
			tbl[page_table_index] = address | 7;
		}
		else
		{
			printf("Segmentation fault.\n");
			hlt();
		}
	}

	set_cr3(v_cr3);
}

/*
 * 缺页申请
 * 向内核申请一个指定页号的页面，如果些页在MMAP中为“可用”则可申请，如果为“已使用”再判断此页是否可以被换出
 * 如果此页不可以被换出，则返回0，如果可以被换出则尝试换出，如果换出失败则返回0，如果换出成功则返回1
 */
int alloc_page_no(u32 page_no)
{
	return 1;
}

int page_swap_in(u32 page_no, u32 sec_no)
{
	return 1;
}
