/*
 * sys_call.c
 * Copyright (C) Feb 10, 2014 by lidq
 *
 * 系统中断程序
 *  - 处理异常中断程序
 *  - 处理系统中断
 */

#include <kernel/sys_call.h>

extern s_pcb *pcb_cur;
//可显字符
extern u8 keys[0x53][2];
//全局按键信号量
extern s_sem sem_key;
//shift键按下状态
u8 kb_key_shift = 0;
char *ch_for_get = NULL;

//浮点寄存器数据存储缓冲区
u8 fpu_d[FPU_SIZE] =
{ 0xf, };

/*
 * 除零错
 */
void int_div_error()
{
	set_ds(GDT_INDEX_KERNEL_DS);
	printf("int_div_error.\n");
	hlt();
}

/***
 * 调试异常
 */
void int_debug_error()
{
	set_ds(GDT_INDEX_KERNEL_DS);
	printf("int_debug_error.\n");
	hlt();
}

/***
 * 不可屏蔽中断
 */
void int_nmi()
{
	set_ds(GDT_INDEX_KERNEL_DS);
	printf("int_nmi.\n");
	hlt();
}

/***
 * 断电
 */
void int_power_down()
{
	set_ds(GDT_INDEX_KERNEL_DS);
	printf("int_power_down.\n");
	hlt();
}

/***
 * 上溢出
 */
void int_bound_out()
{
	set_ds(GDT_INDEX_KERNEL_DS);
	printf("int_bound_out.\n");
	hlt();
}

/***
 * 边界检查
 */
void int_bound_check()
{
	set_ds(GDT_INDEX_KERNEL_DS);
	printf("int_bound_check.\n");
	hlt();
}

/*
 * 无效操作码
 */
void int_invalid_opcode()
{
	set_ds(GDT_INDEX_KERNEL_DS);
	printf("int_invalid_opcode.\n");
	hlt();
}

/***
 * 没有浮点运算器
 */
void int_no_fpu()
{
	set_ds(GDT_INDEX_KERNEL_DS);
	set_cr3(PAGE_DIR);

	//打开浮点运算器
	open_fpu();
	//如果是第一次使用fpu不需要从内存恢复
	if (pcb_cur->is_need_fpu == 0)
	{
		//设置为需要fpu
		pcb_cur->is_need_fpu = 1;
	}
	else
	{
		//恢复内存区数据到浮点寄存器
		mmcopy(pcb_cur->fpu_data, fpu_d, FPU_SIZE);
		//恢复内存区数据到浮点寄存器
		restore_fpu(fpu_d);
	}

	u32 cr3 = pcb_cur->tss.cr3;
	set_cr3(cr3);
	set_ds(0xf);
}

/***
 * 双重错误
 */
void int_double_error()
{
	set_ds(GDT_INDEX_KERNEL_DS);
	printf("int_double_error.\n");
	hlt();
}

/***
 * 浮点运算器段超限
 */
void int_fpu_out()
{
	set_ds(GDT_INDEX_KERNEL_DS);
	printf("int_fpu_out.\n");
	hlt();
}

/***
 * 无效TSS
 */
void int_tss_error()
{
	//set_ds(GDT_INDEX_KERNEL_DS);
	printf("int_tss_error.\n");
	hlt();
}

/***
 * 无效段
 */
void int_section_error()
{
	set_ds(GDT_INDEX_KERNEL_DS);
	printf("int_section_error.\n");
	hlt();
}

/***
 * 无效栈
 */
void int_stack_error()
{
	set_ds(GDT_INDEX_KERNEL_DS);
	printf("int_stack_error.\n");
	hlt();
}

/*
 * 一般保护错误
 */
void int_protection_error()
{
	set_ds(GDT_INDEX_KERNEL_DS);
	printf("int_protection_error.\n");
	hlt();
}

/***
 * 页错误
 */
void int_page_error(u32 error_code)
{
	set_ds(GDT_INDEX_KERNEL_DS);
	set_cr3(PAGE_DIR);

	u32 error_addr = cr2();
	page_error(error_code, error_addr);

	u32 cr3 = pcb_cur->tss.cr3;
	set_cr3(cr3);
	set_ds(0xf);
}

/***
 * 浮点运算器错误
 */
void int_fpu_error()
{
	set_ds(GDT_INDEX_KERNEL_DS);
	printf("int_fpu_error.\n");
	hlt();
}

/*
 * 时钟中断
 */
void int_timer()
{
	//在时钟中断时并没有切换ds和cr3寄存器
	set_ds(GDT_INDEX_KERNEL_DS);
	set_cr3(PAGE_DIR);

	//释放已停止进程
	pcb_release();
	//处理等待链表
	list_sleep_change();
	//任务调度算法
	schedule();
}

/*
 * 键盘中断
 */
void int_keyboard()
{
	set_ds(GDT_INDEX_KERNEL_DS);
	set_cr3(PAGE_DIR);

	//取得扫描码
	u8 scan_code = inb_p(0x60);
	//取得按下、抬起状态
	u8 status = scan_code >> 7;
	//扫描码的索引
	u8 key_ind = scan_code & 0x7f;
	//shift按下
	if ((key_ind == KEY_LEFT_SHIFT || key_ind == KEY_RIGHT_SHIFT) && status == 0)
	{
		kb_key_shift = 0x1;
	}
	//shift抬起
	else if ((key_ind == KEY_LEFT_SHIFT || key_ind == KEY_RIGHT_SHIFT) && status == 1)
	{
		kb_key_shift = 0x0;
	}
	else if (status == 0)
	{
		if (ch_for_get != NULL)
		{
			//得到按键
			char ch = keys[key_ind - 1][kb_key_shift];
			//为请求程序设置按键
			*ch_for_get = ch;
			//清空数据区
			ch_for_get = NULL;
			//pcb_wakeup_key();
		}
	}
	//清除键盘状态可以接受新按键
	outb_p(scan_code & 0x7f, 0x61);
	//通知PIC1可以接受新中断
	outb_p(0x20, 0x20);

	u32 cr3 = pcb_cur->tss.cr3;
	set_cr3(cr3);
	set_ds(0xf);
}

void* addr_parse(u32 cr3, void *p)
{
	u32 addr = (u32) p;
	u32 *page_dir = (u32 *) cr3;
	//页目录索引
	u32 page_dir_index = (addr >> 22) & 0x3ff;
	//页表索引
	u32 page_table_index = (addr >> 12) & 0x3ff;
	u32 *page_tbl = (u32 *) (page_dir[page_dir_index] & 0xfffff000);
	void *p_addr = (void *) ((page_tbl[page_table_index] & 0xfffff000) | (addr & 0xfff));
	return p_addr;
}

void sys_process(int *params)
{
	set_ds(GDT_INDEX_KERNEL_DS);
	set_cr3(PAGE_DIR);
	u32 cr3 = pcb_cur->tss.cr3;
	params = addr_parse(cr3, params);

	//载入可执行文件并创建进程
	if (params[0] == 0)
	{
		char *path = (char *) params[1];
		char *par_s = (char *) params[2];
		u32 *sem_addr = (u32 *) params[3];

		path = addr_parse(cr3, path);
		par_s = addr_parse(cr3, par_s);
		sem_addr = addr_parse(cr3, sem_addr);

		s_pcb *pcb = load_process(path, par_s);
		*sem_addr = (u32) pcb->sem_shell;
	}
	//停止进程
	else if (params[0] == 1)
	{
		pcb_stop(pcb_cur);
	}
	//msleep等待
	else if (params[0] == 2)
	{
		int ms = params[1];
		pcb_sleep(pcb_cur, ms);
	}
	//创建线程
	else if (params[0] == 3)
	{
		s_pthread *p = (s_pthread *) params[1];
		void *function = (void *) params[2];
		void *args = (void *) params[3];

		p = addr_parse(cr3, p);
		function = addr_parse(cr3, function);
		args = addr_parse(cr3, args);

		create_pthread(pcb_cur, p, function, args);
	}

	set_cr3(cr3);
	set_ds(0xf);
}

void sys_semaphore(int *params)
{
	set_ds(GDT_INDEX_KERNEL_DS);
	set_cr3(PAGE_DIR);
	u32 cr3 = pcb_cur->tss.cr3;
	params = addr_parse(cr3, params);

	//P
	if (params[0] == 0)
	{
		s_sem *sem = (s_sem *) (params[1]);
		int *ret = (int *) (params[2]);

		sem = addr_parse(cr3, sem);
		ret = addr_parse(cr3, ret);

		*ret = pcb_sem_P(pcb_cur, sem);
	}
	//V
	else if (params[0] == 1)
	{
		s_sem *sem = (s_sem *) (params[1]);
		int *ret = (int *) (params[2]);

		sem = addr_parse(cr3, sem);
		ret = addr_parse(cr3, ret);

		*ret = pcb_sem_V(pcb_cur, sem);
	}
	//获取全局信号量
	else if (params[0] == 2)
	{
		int type = (int) params[1];
		//按键信号量
		if (type == 0)
		{
			s_sem **sem = (s_sem **) params[2];
			sem = addr_parse(cr3, sem);
			*sem = &sem_key;
		}
		//IDE设备信号量
		else if (type == 1)
		{

		}
	}
	//shell pcb P
	else if (params[0] == 0x10)
	{
		s_sem *sem = (s_sem *) (params[1]);
		int *ret = (int *) (params[2]);
		ret = addr_parse(cr3, ret);
		*ret = pcb_sem_P(pcb_cur, sem);
	}
	//shell pcb V
	else if (params[0] == 0x11)
	{
		s_sem *sem = (s_sem *) (params[1]);
		int *ret = (int *) (params[2]);
		ret = addr_parse(cr3, ret);
		*ret = pcb_sem_V(pcb_cur, sem);
	}
	//取得shell 运行的pcb的信号量
	else if (params[0] == 0x12)
	{
		u32 *sem_addr = (u32 *) (params[1]);
		sem_addr = addr_parse(cr3, sem_addr);
		*sem_addr = (u32) pcb_cur->sem_shell;
	}

	set_cr3(cr3);
	set_ds(0xf);
}

void sys_stdio(int *params)
{
	set_ds(GDT_INDEX_KERNEL_DS);
	set_cr3(PAGE_DIR);
	u32 cr3 = pcb_cur->tss.cr3;
	params = addr_parse(cr3, params);

	//显示字符
	if (params[0] == 0)
	{
		putchar((char) params[1]);
	}
	//显示字符串
	else if (params[0] == 1)
	{
		int *count = (int *) params[2];
		count = addr_parse(cr3, count);
		char *str = (char *) params[1];
		str = addr_parse(cr3, str);
		*count = puts(str);
	}
	//getchar
	else if (params[0] == 0x10)
	{
		char *ch = (char *) params[1];
		ch = addr_parse(cr3, ch);
		ch_for_get = ch;
		//pcb_wait_key(pcb_cur);
	}
	//backspace
	else if (params[0] == 0x11)
	{
		backspace();
	}

	set_cr3(cr3);
	set_ds(0xf);
}

void sys_stdlib(int *params)
{
	set_ds(GDT_INDEX_KERNEL_DS);
	set_cr3(PAGE_DIR);
	u32 cr3 = pcb_cur->tss.cr3;

	params = addr_parse(cr3, params);
	if (params[0] == 0)
	{
		int *ret = (int*) params[1];
		ret = addr_parse(cr3, ret);
		*ret = rand();
	}
	else if (params[0] == 1)
	{
		srand(params[1]);
	}
	else if (params[0] == 2)
	{
		int *ret = (int*) params[3];
		ret = addr_parse(cr3, ret);
		int min = (int) params[1];
		int max = (int) params[2];
		*ret = random(min, max);
	}
	else if (params[0] == 0x10)
	{
		int size = params[1];
		void **addr = (void **) params[2];
		addr = addr_parse(cr3, addr);
		/*
		 * 注意，pcb_malloc返回的是pcb的一个逻辑地址，
		 * 而addr本身是一个需要从逻辑地址转为物理地址的二级指针地址
		 */
		*addr = pcb_malloc(pcb_cur, size);
	}
	else if (params[0] == 0x11)
	{
		//注意，addr是个逻辑地址，不需要转换为物理地址
		void* addr = (void *) params[1];
		pcb_free(pcb_cur, addr);
	}

	set_cr3(cr3);
	set_ds(0xf);
}

void sys_hd_rw(int *params)
{
	set_ds(GDT_INDEX_KERNEL_DS);
	set_cr3(PAGE_DIR);
	u32 cr3 = pcb_cur->tss.cr3;

	params = addr_parse(cr3, params);
	if (params[0] == 0)
	{
		u32 lba = (u32) params[1];
		u8 com = (u32) params[2];
		u8 *buff = (u8 *) params[3];
		buff = addr_parse(cr3, buff);
		hd_rw(lba, com, buff);
	}

	set_cr3(cr3);
	set_ds(0xf);
}

void sys_pts(int *params)
{
	set_ds(GDT_INDEX_KERNEL_DS);
	set_cr3(PAGE_DIR);
	u32 cr3 = pcb_cur->tss.cr3;

	params = addr_parse(cr3, params);
	if (params[0] == 0)
	{
		s_pt *pts = (s_pt *) params[1];
		pts = addr_parse(cr3, pts);
		install_swap(pts);
	}
	else if (params[0] == 1)
	{
		s_pt *pts = (s_pt *) params[1];
		pts = addr_parse(cr3, pts);
		char *mount_point = (char *) params[2];
		mount_point = addr_parse(cr3, pts);
		mount_hda(pts, mount_point);
	}
	else if (params[0] == 2)
	{
		void *pts = (void *) params[1];
		pts = addr_parse(cr3, pts);
		install_pts(pts);
	}

	set_cr3(cr3);
	set_ds(0xf);
}
