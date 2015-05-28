/*
 * page.c
 *
 *  Created on: Apr 20, 2015
 *      Author: lidq
 */

#include <kernel/page.h>

extern s_pcb *pcb_cur;

void install_page()
{
	//页目录开始于 [0x600000, 0x601000) ，大小为1024个（每个4字节）共4096字节
	u32 *page_dir = ((u32 *) PAGE_DIR);
	//页表1开始于0x600000 + 0x1000 = 0x601000
	u32 *page_table = ((u32 *) PAGE_TABLE);
	//用于内存地址计算
	u32 address = 0;
	//处理所有页目录
	//页表开始于 [0x601000, 0xa01000) ，大小为4M
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

	//设置cr3寄存器为页目录的值
	set_cr3(PAGE_DIR);
	//开启内存分页
	open_mm_page();
}

void page_error(u32 pid, u32 error_code)
{
	//取得页面错误地址
	u32 v_cr3 = cr3();
	set_cr3(PAGE_DIR);
	u32 error_addr = cr2();

//	if (error_addr % 0x100000 == 0)
//	{
//	printf("Missing Page: %x\n", error_addr);
//		hlt();
//	}

//	if (error_addr >= 0x20000000)
//	{
//		printf("Missing Page: %x\n", error_addr);
//	}

	if (error_code == 7)
	{
		pcb_stop(pcb_cur);
//		printf("Segmentation fault.\n");
//		hlt();
	}

	//页号
	u32 page_no = error_addr / 0x1000;
	//面目录
	u32 *page_dir = (u32 *) v_cr3;
	//页目录索引
	u32 page_dir_index = (error_addr >> 22) & 0x3ff;
	//页表索引
	u32 page_table_index = (error_addr >> 12) & 0x3ff;
	//页表
	u32 *tbl = NULL;
	//如果页表尚未分配则申请一个页表
	if ((page_dir[page_dir_index] & 0x1) == 0)
	{
		//申请一个普通可用页面
		tbl = alloc_page(pid, 1, 0, 0);
		//如果申请失败
		if (tbl == NULL)
		{
			//申请一个可用的物理页面
			u32 ph_page_no = alloc_page_ph(pid);
			//如果申请成功
			if (ph_page_no != 0)
			{
				tbl = (u32 *) (ph_page_no * 0x1000);
			}
			//如果申请失败
			else
			{
				pcb_stop(pcb_cur);
//				printf("Segmentation fault.\n");
//				hlt();
			}
		}
		//处理新分配的页表所表示的页面均不在内存中
		for (int i = 0; i < 1024; i++)
		{
			tbl[i] = 6;
		}
		//设置页目录
		page_dir[page_dir_index] = (u32) tbl | 7;
	}
	//如果页表已分配
	else
	{
		//取得页表地址
		tbl = (u32 *) (page_dir[page_dir_index] & 0xfffff000);
	}

	//将10、11bit值取出
	u32 userdef = tbl[page_table_index] & 0xc00;

	//如果此页面并没有被换出，swap状态为0
	if ((tbl[page_table_index] >> 9 & 0x1) == 0)
	{

		u32 ph_page_no = 0;
		u32 shared = 0;
		u32 share_addr = 0;
		//如果缺页申请失败
		if (alloc_page_no(pid, page_no, &ph_page_no, &shared, &share_addr) == 0)
		{
			pcb_stop(pcb_cur);
//			printf("Segmentation fault.\n");
//			hlt();
		}
		else
		{

			u32 address = ph_page_no * 0x1000;
			if (shared == 1)
			{
				address = share_addr;
				//共享时页面为只读
				tbl[page_table_index] = address | 5 | userdef;
			}
			else
			{
				tbl[page_table_index] = address | 7 | userdef;
			}
		}
	}
	//如果此页面被已经换出则要从外存换回此页面到内存
	else
	{
		u32 sec_no = tbl[page_table_index] >> 12;
		u32 ph_page_no = 0;
		/*
		 * 如果换入成功，在执行page_swap_in时，
		 * tbl[page_table_index]的前20位存放的是此页被换出到外存的逻辑扇区号sec_no
		 * 在读取外存扇区时sec_no要乘以8，因为是4k对齐，8个512B的扇区大小为4096B=4K
		 */
		if (page_swap_in(page_no, sec_no, pid, &ph_page_no) == 1)
		{
			/*
			 * 这里的一条赋值语句有以下3个作用：
			 * 1.修改页表前20位为线性地址
			 * 2.修改swap位为0，表示没有被换入
			 * 3.修改后3位为在内存中，并可读写
			 * 这里需要修改：
			 * 对于物理内存内的地址按物理地址分配，对于大于物理内存的地址按alloc_page申请的地址来分配
			 */
			u32 address = ph_page_no * 0x1000;
			tbl[page_table_index] = address | 7 | userdef;
		}
		//如果换回内存页失败
		else
		{
			pcb_stop(pcb_cur);
//			printf("Segmentation fault.\n");
//			hlt();
		}
	}
	//恢复cr3
	set_cr3(v_cr3);
}

/*
 * 缺页申请
 * 向内核申请一个指定页号的页面，如果些页在MMAP中为“可用”则可申请，如果为“已使用”再判断此页是否可以被换出
 * 如果此页不可以被换出，则返回0，如果可以被换出则尝试换出，如果换出失败则返回0，如果换出成功则返回1
 */
int alloc_page_no(u32 pid, u32 page_no, u32 *page_no_ret, u32 *shared, u32 *share_addr)
{
	*shared = 0;
	*share_addr = 0;

	*page_no_ret = page_no;
	//取得此页的状态
	u8 status = mmap_status(page_no);
	//如果第0位为0,说明页面未使用
	if ((status & 0x1) == 0)
	{
		//设置此页为“已使用”、“可换出”
		set_mmap_status(page_no, MM_USED | MM_CAN_SWAP);
		//设置此页的使用者id，即pid
		set_map_process_id(page_no, pid);

		//如果超出物理页面数
		if (page_no >= MAP_SIZE)
		{
			//申请一个可用的物理页面号
			u32 ph_page_no = alloc_page_ph(pid);
			if (ph_page_no == 0)
			{
				return 0;
			}
			*page_no_ret = ph_page_no;
		}
		//返回成功
		return 1;
	}
	//如果第0位为1,说明页面已使用
	else
	{
		//尝试将此页面换出到外存
		if (page_swap_out(page_no) == 0)
		{
			//printf("out NG %x\n", page_no);
			//如果页面共享失败
			if (page_share(page_no, share_addr) == 0)
			{
				return 0;
			}
			else
			{
				*shared = 1;
				return 1;
			}
		}
		else
		{
			//printf("out OK %x\n", page_no);
			//设置此页为“已使用”、“可换出”
			set_mmap_status(page_no, MM_USED | MM_CAN_SWAP);
			//设置此页的使用者id为0
			set_map_process_id(page_no, pid);

			//如果超出物理页面数
			if (page_no >= MAP_SIZE)
			{
				//申请一个可用的物理页面号
				u32 ph_page_no = alloc_page_ph(pid);
				if (ph_page_no == 0)
				{
					return 0;
				}
				*page_no_ret = ph_page_no;
			}
			return 1;
		}
	}
}

/*
 * 换出页面，向交换分区申请一个可用外存页面号
 * 将当前页面数据写入交换分区
 */
int page_swap_out(u32 page_no)
{
	//取得此页的状态
	u8 status = mmap_status(page_no);
	//如果未使用
	if ((status & 0x1) == 0)
	{
		//直接返回成功
		return 1;
	}

	//如果页面不可以换出
	if ((status & 2) == MM_NO_SWAP)
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

			//设置此页为“未使用”、“可换出”
			set_mmap_status(page_no, MM_FREE | MM_CAN_SWAP);
			//设置此页的使用者id为0
			set_map_process_id(page_no, 0);

			//如果超出物理页面数
			if (page_no >= MAP_SIZE)
			{
				//计算物理页面号
				u32 ph_page_no = (u32) page_data / 0x1000;
				//设置此页为“未使用”、“可换出”
				set_mmap_status(ph_page_no, MM_FREE | MM_CAN_SWAP);
				//设置此页的使用者id为0
				set_map_process_id(ph_page_no, 0);
			}
			//返回成功
			return 1;

		}
	}
}

/*
 * 换回页面，将交换分区的数据换回至内存
 */
int page_swap_in(u32 page_no, u32 sec_no, u32 pid, u32 *page_no_ret)
{
	*page_no_ret = page_no;

	void* page_data = (void *) (page_no * 0x1000);

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
		//如果超出物理页面数
		if (page_no >= MAP_SIZE)
		{
			//申请一个可用的物理页面号
			u32 ph_page_no = alloc_page_ph(pid);
			if (ph_page_no == 0)
			{
				return 0;
			}
			*page_no_ret = ph_page_no;
			page_data = (void *) (ph_page_no * 0x1000);
		}

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
		//如果换出失败
		if (page_swap_out(page_no) == 1)
		{
			//如果超出物理页面数
			if (page_no >= MAP_SIZE)
			{
				//申请一个可用的物理页面号
				u32 ph_page_no = alloc_page_ph(pid);
				if (ph_page_no == 0)
				{
					return 0;
				}
				*page_no_ret = ph_page_no;
				page_data = (void *) (ph_page_no * 0x1000);
			}

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

	return 0;
}

/*
 * 共享内存页，共享时只读，不能写入，以避免出现内存数据被其它程序破坏
 */
int page_share(u32 page_no, u32 *share_addr)
{
	//取得正在使用这一页面的pid_used
	u32 pid_used = map_process_id(page_no);
	//取得页目录及页表，为换出做准备
	u32 *page_dir = pcb_page_dir(pid_used);

	u32 d_ind = page_no / 1024;
	u32 t_ind = page_no % 1024;

	u32 *tbl = (u32 *) (page_dir[d_ind] & 0xfffff000);

	//取得页面地址4k对齐
	*share_addr = (u32) (tbl[t_ind] & 0xfffff000);

	return 1;
}
