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
u32 malloc_start = 128 * 1024;

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
	for (u32 i = malloc_start; i < 1024 * 512; i++)
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
}
