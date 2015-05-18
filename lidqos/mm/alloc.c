/*
 * alloc.c
 * Copyright (C) Feb 10, 2014 by lidq
 *
 * 内存申请:
 *  - 内存申请
 *  - 内存释放
 */

#ifndef _ALLOC_C_
#define _ALLOC_C_

#include <kernel/alloc.h>

//内存使用位图
u8 *mmap = NULL;

/*
 * 使用map_process存入每一个内存页面所被程序使用情况
 * map_process中存放了使用这个内存页的任务ID所在的
 * 固定内存区域为 [0x200000, 0x600000) 占用4M
 * 对于内核程序所使用的内存页面map_process被设置为0
 * 其它程序所使用的内存页面被设置为其任务ID
 */
u32 *map_process = NULL;
u32 find_index = MMAP_USED_SIZE;

/*
 * install_alloc : 安装申请内存位图
 * return : void
 */
void install_alloc()
{
	//位图所在的固定内存区域为 [0x100000, 0x200000)
	mmap = (u8 *) MMAP;

	for (u32 i = 0; i < MAP_SIZE_LOGIC; i++)
	{
		//16M以下均为已使用
		if (i < MMAP_USED_SIZE)
		{
			//设定内核所占用的1MB内存为已使用
			mmap[i] = (MM_USED | MM_NO_SWAP);
		}
		//剩下的内存为未使用
		else
		{
			mmap[i] = (MM_FREE | MM_CAN_SWAP);
		}
	}
}

//初始化内存页的使用者
void install_used_map()
{
	map_process = (u32 *) MMAP_PRO;
	/*
	 * 初始化map_process，因为map和map_process的大小永远都是一样的所以用MAP_SIZE
	 * map_process所占内存空间为 [0x200000, 0x600000)
	 */
	for (u32 i = 0; i < MAP_SIZE_LOGIC; i++)
	{
		map_process[i] = 0;
	}
}

/*
 * alloc_page : 申请内存页，每页为4KB大小
 *  - int count : 页数
 * return : void*返回申请地址，NULL代表申请失败
 */
void* alloc_page(u32 process_id, int count, int can_swap, int is_dynamic)
{
	//查找内存申请地址
	void *ret = NULL;
	//找到空闲内存页计数
	int num = 0;
	//开始编号
	u32 start_with = 0;
	//从未被分配内存页的地方开始查找
	for (u32 i = MMAP_USED_SIZE; i < MAP_SIZE; i++)
	{
		//如果找到空闲页
		if ((mmap[i] & 0x1) == MM_FREE)
		{
			//设置可分配内存起始编号
			if (start_with == 0)
			{
				ret = (void*) (i * MM_PAGE_SIZE);
				start_with = i;
			}
			num++;
		}
		//如果没有找到空闲页
		else
		{
			//清空变量
			ret = NULL;
			num = 0;
			start_with = 0;
		}
		//找到了可分配内存页，并且找到了预期想要分配的数量
		if (start_with != 0 && num >= count)
		{
			break;
		}
	}
	//设置map的各个内存页的状态为已使用
	for (u32 i = 0; i < count; i++)
	{
//		if (can_swap == 1)
//		{
//			mmap[start_with + i] = (MM_USED | MM_CAN_SWAP);
//		}
//		else
//		{
//			mmap[start_with + i] = (MM_USED | MM_NO_SWAP);
//		}
		mmap[start_with + i] = (MM_USED | (can_swap << 1) | (is_dynamic << 1));

		map_process[i] = process_id;
	}

//	u32 addr = (u32) ret;
//	//页目录索引 / 4M
//	u32 page_dir_index = addr / 0x400000;
//	//页表索引 / 4K
//	u32 page_table_index = (addr % 0x400000) / 0x1000;
//	//页内偏移
//	u32 page_inside = (addr % 0x400000) % 0x1000;
//	//开启分页后的地址计算
//	u32 result = 0;
//	result |= (page_dir_index & 0x3FF) << 22;
//	result |= (page_table_index & 0x3FF) << 12;
//	result |= (page_inside & 0x3FF);
//	ret = (void *) result;

//返回查找到内存地址
	return ret;
}

/*
 * alloc_page : 释放内存页，每页为4KB大小
 *  - void *addr : 释放地址
 *  - int count : 释放页数
 * return : void
 */
void free_page(void *addr, int count)
{
	//释放内存页
	for (int i = 0; i < count; i++)
	{
		//更新map中这些页的状态
		mmap[(u32) (addr + (i * MM_PAGE_SIZE)) / MM_PAGE_SIZE] = (MM_FREE | MM_CAN_SWAP | MM_NO_DYNAMIC);
		map_process[i] = 0;
	}
}

/*
 * alloc_mm : 申请内存
 *  - int size : 申请大小
 * return : void*返回申请地址，NULL代表申请失败
 */
void* alloc_mm(int size)
{
	//如果申请内存大小大于一个页大小，则按整页分配内存
	if (size > (MM_PAGE_SIZE - 128))
	{
		//计算有多少个内存页
		u32 count = (size / MM_PAGE_SIZE);
		//如果有余数，说明要多分配一个页面
		if (size % MM_PAGE_SIZE != 0)
		{
			count++;
		}
		//按整页分配内存
		return alloc_page(0, count, 0, 1);
	}

	//4字节对齐分配内存
	u32 alloc_size = size / 4;
	//如果有余数，说明要多分配一个4字节
	if (size % 4 > 0)
	{
		alloc_size++;
	}

	//i为map下标，j为页内map下标，k为页内字节位偏移，c为查找count
	int i, j, k, c = 0, break_status = 0, run_time = 0;
	//is为起始map下标，js为页内起始map下标，ks为面内起始字节偏移
	int is = -1, js = -1, ks = -1;
	//从未被分配内存页的地方开始查找
	for (i = MMAP_USED_SIZE; i < MAP_SIZE && !break_status; i++)
	{
		//如果是未使用或已使用但是动态内存
		if ((mmap[i] & 0x1) == MM_FREE || ((mmap[i] & 0x1) == MM_USED && (mmap[i] & 0x4) == MM_DYNAMIC))
		{
			is = -1;
			js = -1;
			ks = -1;
			c = 0;

			//取得页内map位图
			u8 *mpmap = (u8*) (i * MM_PAGE_SIZE);
			//跳过前4个字节，并小于128个字节中查找页内位图map
			//一个页面为4096字节，前128字节为页内位图，0x80 * 0x8 * 0x4 = 0x1000 = 4096
			//这128个字节占用页内位图为 0x80 / 0x8 / 0x4 = 0x4字节，所以要跳过前4个字节
			for (j = 4; j < 128 && !break_status; j++)
			{
				//字节偏移从0到7字节来查找可用内存
				for (k = 0; k < 8 && !break_status; k++)
				{
					//如果可以使用
					if (((mpmap[j] >> k) & 0x1) == 0)
					{
						//设置各项起始号
						if (is == -1 && js == -1 && ks == -1)
						{
							is = i;
							js = j;
							ks = k;
						}
						//已找到数量自增
						c++;
					}
					//如果不可用
					else
					{
						//各项起始号清除
						is = -1;
						js = -1;
						ks = -1;
						c = 0;
					}
					//如果找到可分配内存数量达到预先要申请的数量
					if (c >= alloc_size)
					{
						//跳出
						break_status = 1;
					}
				}
			}
		}
		//如果内存页为已使用
		else
		{
			//各项起始号清除
			is = -1;
			js = -1;
			ks = -1;
			c = 0;
		}
	}
	//清空分配数
	c = 0;
	//正式分配内存
	if (break_status == 1 && is != -1 && js != -1 && ks != -1)
	{
		//从内存位图开始
		for (i = is; i < MAP_SIZE; i++)
		{
			//如果是可分配内存
			if ((mmap[i] & 0x1) == MM_FREE || ((mmap[i] & 0x1) == MM_USED && (mmap[i] & 0x4) == MM_DYNAMIC))
			{
				//取得页内位图
				u8 *mpmap = (u8*) (i * MM_PAGE_SIZE);
				//从页内位图的第4个字节开始
				for (j = 4; j < 128; j++)
				{
					//如果是第1次，找到起始地址
					if (run_time == 0)
					{
						j = js;
					}
					//页内偏移
					for (k = 0; k < 8; k++)
					{
						//如果是第1次，找到起始偏移
						if (run_time == 0)
						{
							k = ks;
						}
						//更新页内位图
						mpmap[j] |= (1 << k);
						//找到数量自增
						c++;
						//次数加1
						run_time++;
						//找到预期的申请数量
						if (c >= alloc_size)
						{
							//将此内存页设定为动态分配
							mmap[i] = MM_USED | MM_NO_SWAP | MM_DYNAMIC;

							u32 ret = (is * MM_PAGE_SIZE + (js * 8 * 4) + (ks * 4));
//							//printf("addr %x\n", ret);
//							u32 addr = (u32) ret;
//							//页目录索引 / 4M
//							u32 page_dir_index = addr / 0x400000;
//							//printf("p_dir %x\n", page_dir_index);
//							//页表索引 / 4K
//							u32 page_table_index = (addr % 0x400000) / 0x1000;
//							//printf("p_tab %x\n", page_table_index);
//							//页内偏移
//							u32 page_inside = (addr % 0x400000) % 0x1000;
//							//printf("p_addr %x\n", page_inside);
//							//开启分页后的地址计算
//							u32 result = 0;
//							result |= (page_dir_index & 0x3FF) << 22;
//							result |= (page_table_index & 0x3FF) << 12;
//							result |= (page_inside & 0x3FF);
//							ret = result;

							//返回申请内存地址
							return (void*) ret;
						}
					}
				}
			}
		}
	}

	//返回空指针
	return NULL;
}

/*
 * free_mm : 释放内存
 *  - void* addr : 释放地址
 *  - int size : 释放大小
 * return : void
 */
void free_mm(void* addr, int size)
{
	//如果大小大于一个内存页大小，则按整内存页释放
	if (size > (MM_PAGE_SIZE - 128))
	{
		//计算有多少个内存页
		int count = (size / MM_PAGE_SIZE);
		//如果有余数说明要多释放一个页
		if (size % MM_PAGE_SIZE != 0)
		{
			count++;
		}
		//释放内存页
		free_page(addr, count);
		return;
	}

	//按4字节对齐释放
	int alloc_size = size / 4;
	//如果有余数则说明要多释放一个4字节空间
	if (size % 4 > 0)
	{
		alloc_size++;
	}
	//释放内存起始号
	int is, js, ks, run_time = 0, count = 0;
	//计算位图起始号
	is = (u32) addr / MM_PAGE_SIZE;
	//计算页内位图起始号
	js = ((u32) addr % MM_PAGE_SIZE) / (8 * 4);
	//计算页内位图位偏移起始号
	ks = ((u32) addr % MM_PAGE_SIZE) % (8 * 4) / 4;
	//从内存页开始
	for (int i = is; i < MAP_SIZE; i++)
	{
		//取得页内偏移
		u8 *mpmap = (u8*) (i * MM_PAGE_SIZE);
		//从页内位图中第4个开始
		for (int j = 4; j < 128; j++)
		{
			//如果是第1次释放
			if (run_time == 0)
			{
				j = js;
			}
			//从页内偏移位开始
			for (int k = 0; k < 8; k++)
			{
				//如果是第1次释放
				if (run_time == 0)
				{
					k = ks;
				}
				//设定页内位图为动态可用内存
				mpmap[j] &= (~(1 << k));
				//数量自增
				count++;
				//次数自增
				run_time++;
				//如果已释放了预期大小的内存
				if (count >= alloc_size)
				{
					//完成，返回
					return;
				}
			}
		}
	}
}

u8 mmap_status(u32 page_no)
{
	return mmap[page_no];
}

u32 map_process_id(u32 page_no)
{
	return map_process[page_no];
}

void set_mmap_status(u32 page_no, u8 status)
{
	mmap[page_no] = status;
}

void set_map_process_id(u32 page_no, u32 pid)
{
	map_process[page_no] = pid;
}

/*
 * 申请物理内存页
 */
u32 alloc_page_ph(u32 pid)
{
	u32 ret = 0;
	//从未被分配内存页的地方开始查找
	for (u32 i = MMAP_USED_SIZE; i < MAP_SIZE && ret == 0; i++)
	{
		//如果找到空闲页
		if ((mmap[find_index] & 0x3) == (MM_FREE | MM_CAN_SWAP))
		{
			set_mmap_status(find_index, MM_USED | MM_CAN_SWAP);
			set_map_process_id(find_index, pid);
			ret = find_index;
		}
		find_index++;
		if (find_index >= MAP_SIZE)
		{
			find_index = MMAP_USED_SIZE;
		}

	}
	//如果没有free页面，则再找可换出页面
	for (u32 i = MMAP_USED_SIZE; i < MAP_SIZE && ret == 0; i++)
	{
		//如果找到可换出页面
		if ((mmap[find_index] & 0x3) == (MM_USED | MM_CAN_SWAP))
		{
			//将此物理页面换出
			if (page_swap_out(find_index) == 0)
			{
				return 0;
			}
			set_mmap_status(find_index, MM_USED | MM_CAN_SWAP);
			set_map_process_id(find_index, pid);
			ret = find_index;
		}

		find_index++;
		if (find_index >= MAP_SIZE)
		{
			find_index = MMAP_USED_SIZE;
		}
	}

	return ret;
}

#endif
