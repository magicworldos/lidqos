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
	u32 v_cr3 = cr3();
	set_cr3(PAGE_DIR);
	u32 error_addr = cr2();
	if (error_code == 7)
	{
		printf("Segmentation fault.\n");
		hlt();
	}

	//页号
	u32 page_no = error_addr / 0x1000;

	if (page_no >= MAP_SIZE)
	{
		printf("Segmentation fault.\n");
		hlt();
	}

	u32 *page_dir = (u32 *) v_cr3;

	//页目录索引
	u32 page_dir_index = (error_addr >> 22) & 0x3ff;
	//页表索引
	u32 page_table_index = (error_addr >> 12) & 0x3ff;

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
		tbl = (u32 *) (page_dir[page_dir_index] & 0xfffff000);
	}

	//使地址4k对齐
	u32 address = error_addr & 0xfffff000;

	//如果此页面并没有被换出，swap状态为0
	if ((tbl[page_table_index] >> 9 & 0x1) == 0)
	{
		//如果缺页申请成功
		if (alloc_page_no(pid, page_no) == 0)
		{
			printf("Segmentation fault.\n");
			hlt();
		}
		else
		{
			tbl[page_table_index] = address | 7;
		}
	}
	//如果此页面被已经换出则要从外存换回此页面到内存
	else
	{
		u32 sec_no = tbl[page_table_index] >> 12;
		/*
		 * 如果换入成功，在执行page_swap_in时，
		 * tbl[page_table_index]的前20位存放的是此页被换出到外存的逻辑扇区号sec_no
		 * 在读取外存扇区时sec_no要乘以8，因为是4k对齐，8个512B的扇区大小为4096B=4K
		 */
		if (page_swap_in(page_no, sec_no, pid))
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
int alloc_page_no(u32 pid, u32 page_no)
{
	//取得此页的状态
	u8 status = mmap_status(page_no);
	//如果第0位为0,说明页面未使用
	if ((status & 0x1) == 0)
	{
		//设置此页为“已使用”、“可换出”
		set_mmap_status(page_no, MM_USED | MM_CAN_SWAP);
		//设置此页的使用者id，即pid
		set_map_process_id(page_no, pid);
		//返回成功
		return 1;
	}
	//如果第0位为1,说明页面已使用
	else
	{
		//如果页面不可以换出
		if (((status >> 1) & 0x1) == 0)
		{
			//返回失败
			return 0;
		}
		//如果页面可以换出
		else
		{
			//向外存申请8个扇区用来存放一个4k的页面
			u32 sec_no = swap_alloc_sec();
			//如果申请扇区失败
			if (sec_no == 0xffffffff)
			{
				//返回失败
				return 0;
			}
			else
			{
				//如果换出失败
				if (page_swap_out(page_no, sec_no) == 0)
				{
					//释放扇区
					swap_free_sec(sec_no);

					return 0;
				}
				else
				{
					//设置此页为“已使用”、“可换出”
					set_mmap_status(page_no, MM_USED | MM_CAN_SWAP);
					//设置此页的使用者id，即pid
					set_map_process_id(page_no, pid);
					//返回成功
					return 1;
				}
			}
		}
	}

	//返回失败
	return 0;
}

int page_swap_out(u32 page_no, u32 sec_no)
{
	//取得正在使用这一页面的pid_used
	u32 pid_used = map_process_id(page_no);
	//取得页目录及页表，为换出做准备
	u32 *page_dir = pcb_page_dir(pid_used);

	u32 d_ind = page_no / 1024;
	u32 t_ind = page_no % 1024;

	u32 *tbl = (u32 *) (page_dir[d_ind] & 0xfffff000);

	//取得页面地址4k对齐
	void* page_data = (void *) (tbl[t_ind] & 0xfffff000);
	//将页面数据写入外存
	swap_write_page(sec_no, page_data);

	/*
	 * 设置原任务的此页面失效
	 * 将页表中的地址域放入逻辑扇区号
	 * 将swap状态位置为1表示此页面被换出到外存
	 */
	tbl[t_ind] = 6;
	tbl[t_ind] |= (0x1 << 9);
	tbl[t_ind] |= sec_no << 12;

	//返回成功
	return 1;
}

int page_swap_in(u32 page_no, u32 sec_no, u32 pid)
{
	//取得此页的状态
	u8 status = mmap_status(page_no);

	//如果页面不可以换出
	if (((status >> 1) & 0x1) == 0)
	{
		//返回失败
		return 0;
	}

	//如果第0位为0,说明页面未使用
	if ((status & 0x1) == 0)
	{
		void *page_data = (void *) (page_no * 0x1000);
		//将页面数据从外存读入到内存
		swap_read_page(sec_no, page_data);
		//释放扇区
		swap_free_sec(sec_no);
		//返回成功
		return 1;
	}

	//如果第0位为0,说明页面已使用
	if ((status & 0x1) == 1)
	{

		//向外存申请8个扇区用来存放一个4k的页面
		u32 sec_no_out = swap_alloc_sec();
		//如果申请扇区失败
		if (sec_no_out == 0xffffffff)
		{
			//返回失败
			return 0;
		}
		else
		{
			//如果换出失败
			if (page_swap_out(page_no, sec_no_out) == 0)
			{
				//释放扇区
				swap_free_sec(sec_no_out);

				return 0;
			}
			else
			{
				void *page_data = (void *) (page_no * 0x1000);
				//将页面数据从外存读入到内存
				swap_read_page(sec_no, page_data);
				//释放扇区
				swap_free_sec(sec_no);
				//设置此页为“已使用”、“可换出”
				set_mmap_status(page_no, MM_USED | MM_CAN_SWAP);
				//设置此页的使用者id，即pid
				set_map_process_id(page_no, pid);

				//返回成功
				return 1;
			}
		}
	}

	return 0;
}
