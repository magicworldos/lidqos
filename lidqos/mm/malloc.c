/*
 * alloc.c
 * Copyright (C) Feb 10, 2014 by lidq
 *
 * 内存申请:
 *  - 内存申请
 *  - 内存释放
 */

#include <kernel/malloc.h>

//从512M开始向上，到2G为止，2G以上为pcb的栈使用
#define  MALLOC_START  	(128 * 1024)
#define  MALLOC_END  	(512 * 1024)

void* palloc(s_pcb *pcb, int page)
{
	u32 *p_tbl = pcb->page_tbl;
	//查找内存申请地址
	void *ret = NULL;
	//找到空闲内存页计数
	int num = 0;
	//开始编号
	u32 start_with = 0;
	/*
	 * 从是从物理内存的最大值未被分配内存页的地方开始查找
	 */
	for (u32 i = MALLOC_START; i < MALLOC_END; i++)
	{
		//如果找到空闲页
		if (((p_tbl[i] >> 10) & 0x1) == 0)
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
		if (start_with != 0 && num >= page)
		{
			break;
		}
	}
	//设置map的各个内存页的状态为已使用
	for (u32 i = 0; i < page; i++)
	{
		//将10bit置为1,表示已使用
		p_tbl[start_with + i] |= (1 << 10);
		//将11bit置为0,表示非动态页
		p_tbl[start_with + i] &= 0xfffff7ff;
	}
	return ret;
}

void pfree(s_pcb *pcb, void *addr, int page)
{
	u32 *p_tbl = pcb->page_tbl;
	//释放内存页
	for (u32 i = 0; i < page; i++)
	{
		//将10、11bit置为0,表示空闲页
		p_tbl[(u32) (addr + (i * MM_PAGE_SIZE)) / MM_PAGE_SIZE] &= 0xfffff3ff;
	}
}

void* malloc(s_pcb *pcb, int size)
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
		return palloc(pcb, count);
	}

	u32 *p_tbl = pcb->page_tbl;

	//4字节对齐分配内存
	u32 alloc_size = size / 4;
	//如果有余数，说明要多分配一个4字节
	if (size % 4 != 0)
	{
		alloc_size++;
	}

	//i为map下标，j为页内map下标，k为页内字节位偏移，c为查找count
	int i, j, k, c = 0, break_status = 0, run_time = 0;
	//is为起始map下标，js为页内起始map下标，ks为面内起始字节偏移
	int is = -1, js = -1, ks = -1;
	//从未被分配内存页的地方开始查找
	for (u32 i = MALLOC_START; i < MALLOC_END && !break_status; i++)
	{

		if ((p_tbl[i] & 1) == 0 && (p_tbl[i] >> 9 & 0x1) == 0)
		{
			u32 address = i * MM_PAGE_SIZE;
			page_error(6, address);
		}
		//如果是未使用或已使用但是动态内存
		if (((p_tbl[i] >> 10) & 0x1) == 0 || (((p_tbl[i] >> 10) & 0x1) == 1 && ((p_tbl[i] >> 11) & 0x1) == 1))
		{
			is = -1;
			js = -1;
			ks = -1;
			c = 0;

			//取得页内位图，如果此页面不存在会触发缺页异常
			u8 *mpmap = (u8 *) (p_tbl[i] & 0xfffff000);
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
		for (i = is; i < MALLOC_END; i++)
		{
			//如果是可分配内存
			if (((p_tbl[i] >> 10) & 0x1) == 0 || (((p_tbl[i] >> 10) & 0x1) == 1 && ((p_tbl[i] >> 11) & 0x1) == 1))
			{
				u8 *mpmap = (u8 *) (p_tbl[i] & 0xfffff000);
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
							p_tbl[i] |= 0xc00;

							u32 ret = (is * MM_PAGE_SIZE + (js * 8 * 4) + (ks * 4));
							//printf("malloc %x\n", ret);
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

void free(s_pcb *pcb, void *addr, int size)
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
		pfree(pcb, addr, count);
		return;
	}

	u32 *p_tbl = pcb->page_tbl;

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
	for (int i = is; i < MALLOC_END; i++)
	{
		//取得页内偏移
		u8 *mpmap = (u8 *) (p_tbl[i] & 0xfffff000);
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
