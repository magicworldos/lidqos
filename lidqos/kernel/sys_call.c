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
//
extern s_sys_var *sys_var;
//当前tty
extern s_tty *tty_cur;

//键按下状态
u8 kb_key_shift = 0;
u8 kb_key_ctrl = 0;
u8 kb_key_alt = 0;

//浮点寄存器数据存储缓冲区
u8 fpu_d[FPU_SIZE] =
{ 0xf, };

int timer = 0;

/*
 * 除零错
 */
void int_div_error()
{
	set_ds(GDT_INDEX_KERNEL_DS);
	printf(0, "int_div_error.\n");
	hlt();
}

/***
 * 调试异常
 */
void int_debug_error()
{
	set_ds(GDT_INDEX_KERNEL_DS);
	printf(0, "int_debug_error.\n");
	hlt();
}

/***
 * 不可屏蔽中断
 */
void int_nmi()
{
	set_ds(GDT_INDEX_KERNEL_DS);
	printf(0, "int_nmi.\n");
	hlt();
}

/***
 * 断电
 */
void int_power_down()
{
	set_ds(GDT_INDEX_KERNEL_DS);
	printf(0, "int_power_down.\n");
	hlt();
}

/***
 * 上溢出
 */
void int_bound_out()
{
	set_ds(GDT_INDEX_KERNEL_DS);
	printf(0, "int_bound_out.\n");
	hlt();
}

/***
 * 边界检查
 */
void int_bound_check()
{
	set_ds(GDT_INDEX_KERNEL_DS);
	printf(0, "int_bound_check.\n");
	hlt();
}

/*
 * 无效操作码
 */
void int_invalid_opcode()
{
	set_ds(GDT_INDEX_KERNEL_DS);
	printf(0, "int_invalid_opcode.\n");
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
	printf(0, "int_double_error.\n");
	hlt();
}

/***
 * 浮点运算器段超限
 */
void int_fpu_out()
{
	set_ds(GDT_INDEX_KERNEL_DS);
	printf(0, "int_fpu_out.\n");
	hlt();
}

/***
 * 无效TSS
 */
void int_tss_error()
{
	//set_ds(GDT_INDEX_KERNEL_DS);
	printf(0, "int_tss_error.\n");
	hlt();
}

/***
 * 无效段
 */
void int_section_error()
{
	set_ds(GDT_INDEX_KERNEL_DS);
	printf(0, "int_section_error.\n");
	hlt();
}

/***
 * 无效栈
 */
void int_stack_error()
{
	set_ds(GDT_INDEX_KERNEL_DS);
	printf(0, "int_stack_error.\n");
	hlt();
}

/*
 * 一般保护错误
 */
void int_protection_error()
{
	set_ds(GDT_INDEX_KERNEL_DS);
	printf(0, "int_protection_error.\n");
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
	printf(0, "int_fpu_error.\n");
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

	//生成随机数种子
	srand(timer++);
	if (timer % 10 == 0)
	{
		//释放已停止进程
		pcb_release();
	}
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
	//ctrl key down
	else if (key_ind == KEY_CTRL && status == 0)
	{
		kb_key_ctrl = 0x1;
	}
	//ctrl key up
	else if (key_ind == KEY_CTRL && status == 1)
	{
		kb_key_ctrl = 0x0;
	}
	//alt key down
	else if (key_ind == KEY_ALT && status == 1)
	{
		kb_key_alt = 0x1;
	}
	//alt key up
	else if (key_ind == KEY_ALT && status == 1)
	{
		kb_key_alt = 0x0;
	}
	//切换tty F1 - F10
	else if (status == 0 && (key_ind - KEY_F1) >= 0 && (key_ind - KEY_F1) < TTY_COUNT && kb_key_ctrl && kb_key_alt && !kb_key_shift)
	{
		switch_tty(key_ind - KEY_F1);
	}
	else if (status == 0)
	{
		char ch = keys[key_ind - 1][kb_key_shift];
		if ((ch >= 0x20 && ch <= 0x7e) || ch == 0x08 || ch == '\n' || ch == '\t')
		{
			if (tty_cur->sem_keybuff_w.value > 0)
			{
				tty_cur->sem_keybuff_w.value--;
				//得到按键

				//为请求程序设置按键
				tty_cur->ch[tty_cur->ch_index_w % TTY_KEY_BUFF_SIZE] = ch;
				tty_cur->ch_index_w++;

				pcb_sem_V(&tty_cur->sem_keybuff_r);
			}
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
		int type = params[3];
		u32 *sem_addr = (u32 *) params[4];
		int *status = (int *) params[5];
		int tty_id = params[6];

		path = addr_parse(cr3, path);
		par_s = addr_parse(cr3, par_s);
		sem_addr = addr_parse(cr3, sem_addr);
		status = addr_parse(cr3, status);

		*sem_addr = 0;
		s_pcb *pcb = load_process(path, par_s, status);
		if (pcb != NULL)
		{
			//设置tty_id
			pcb->tty_id = tty_id;
			*sem_addr = (u32) pcb->sem_shell;
			//type为1时，说明是shell执行普通程序
			if (type == 1)
			{
				//设定普通程序的shell_pid
				pcb->shell_pid = pcb_cur->process_id;
			}
		}
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
	//初始化shell的session
	else if (params[0] == 4)
	{
		s_session *session = (s_session *) params[1];
		session = addr_parse(cr3, session);

		char *cur_path = pcb_cur->session.current_path;
		mmcopy(session, &pcb_cur->session, sizeof(s_session));
		pcb_cur->session.current_path = cur_path;

		char *p = session->current_path;
		p = addr_parse(cr3, p);
		str_copy(p, pcb_cur->session.current_path);
	}
	//取得pcb运行参数
	else if (params[0] == 5)
	{
		char *pars = (char *) params[1];
		pars = addr_parse(cr3, pars);
		str_copy(pcb_cur->pars, pars);
	}
	//取得shell当前目录
	else if (params[0] == 6)
	{
		char *current_path = (char *) params[1];
		current_path = addr_parse(cr3, current_path);
		s_pcb* pcb = pcb_by_id(pcb_cur->shell_pid);
		str_copy(pcb->session.current_path, current_path);
	}
	//取得session
	else if (params[0] == 7)
	{
		s_session *session = (s_session *) params[1];
		session = addr_parse(cr3, session);
		s_pcb* pcb = pcb_by_id(pcb_cur->shell_pid);
		char *p = session->current_path;
		char *pb = p;
		p = addr_parse(cr3, p);
		mmcopy(&pcb->session, session, sizeof(s_session));
		str_copy(pcb->session.current_path, p);
		session->current_path = pb;
	}
	//重新设定shell的current_path
	else if (params[0] == 8)
	{
		char *current_path = (char *) params[1];
		current_path = addr_parse(cr3, current_path);
		s_pcb* pcb = pcb_by_id(pcb_cur->shell_pid);
		str_copy(current_path, pcb->session.current_path);
	}
	//shell自己取得shell当前目录
	else if (params[0] == 9)
	{
		char *current_path = (char *) params[1];
		current_path = addr_parse(cr3, current_path);
		str_copy(pcb_cur->session.current_path, current_path);
	}
	//取得当前tty_id
	else if (params[0] == 10)
	{
		int *tty_id = (int *) params[1];
		tty_id = addr_parse(cr3, tty_id);
		*tty_id = pcb_cur->tty_id;
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

		*ret = pcb_sem_V(sem);
	}
	//获取全局信号量
	else if (params[0] == 2)
	{
		int type = (int) params[1];
		//按键写信号量
		if (type == 0)
		{
			s_sem **sem_w = (s_sem **) params[2];
			sem_w = addr_parse(cr3, sem_w);
			*sem_w = &sys_var->ttys[pcb_cur->tty_id].sem_keybuff_w;
		}
		//按键读信号量
		else if (type == 1)
		{
			s_sem **sem_r = (s_sem **) params[2];
			sem_r = addr_parse(cr3, sem_r);
			*sem_r = &sys_var->ttys[pcb_cur->tty_id].sem_keybuff_r;
		}
	}
	//P
	if (params[0] == 5)
	{
		s_sem *sem = (s_sem *) (params[1]);
		int *ret = (int *) (params[2]);
		ret = addr_parse(cr3, ret);

		*ret = pcb_sem_P(pcb_cur, sem);
	}
	//V
	else if (params[0] == 6)
	{
		s_sem *sem = (s_sem *) (params[1]);
		int *ret = (int *) (params[2]);
		ret = addr_parse(cr3, ret);

		*ret = pcb_sem_V(sem);
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
		*ret = pcb_sem_V(sem);
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

	int tty_id = pcb_cur->tty_id;
	//显示字符
	if (params[0] == 0)
	{
		putchar(tty_id, (char) params[1]);
	}
	//显示字符串
	else if (params[0] == 1)
	{
		int *count = (int *) params[2];
		count = addr_parse(cr3, count);
		char *str = (char *) params[1];
		str = addr_parse(cr3, str);
		*count = puts(tty_id, str);
	}
	//getchar
	else if (params[0] == 0x10)
	{
		char *ch = (char *) params[1];
		ch = addr_parse(cr3, ch);
		//do
		//{
		*ch = tty_cur->ch[tty_cur->ch_index_r % TTY_KEY_BUFF_SIZE];
		tty_cur->ch_index_r++;
		//}
		//while (*ch == 0 && tty_cur->ch_index_r < TTY_KEY_BUFF_SIZE);

	}
	//backspace
	else if (params[0] == 0x11)
	{
		backspace(tty_id);
	}
	else if (params[0] == 0x20)
	{
		int flag = params[1];
		twinkle_cursor(flag);
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
		mount_point = addr_parse(cr3, mount_point);
		mount_hda(pts, mount_point);
	}
	else if (params[0] == 2)
	{
		void *pts = (void *) params[1];
		pts = addr_parse(cr3, pts);
		int pt_count = params[2];
		install_pts(pt_count, pts);
	}

	set_cr3(cr3);
	set_ds(0xf);
}

/*
 * sys_file : 文件操作
 *  - int params[0] :
 *  	0 fopen打开文件
 *  	1 fclose关闭文件
 *  	2 fwrite写文件
 *  	3 fread读文件
 *  	4 fgetch读一个字符
 *  	5 fgetline读一行
 *  	6 fputch写一个字符
 *  	7 fputline写一行
 *  	8 is_eof判断已是文件尾
 * return : void
 */
void sys_file(int *params)
{
	set_ds(GDT_INDEX_KERNEL_DS);
	set_cr3(PAGE_DIR);
	u32 cr3 = pcb_cur->tss.cr3;
	params = addr_parse(cr3, params);

	//fopen打开文件
	if (params[0] == 0)
	{
		char *file = (char *) params[1];
		int fs_mode = params[2];
		u32 uid = params[3];
		u32 gid = params[4];
		s_file *fp = (s_file *) params[5];
		int *status = (int *) params[6];
		file = addr_parse(cr3, file);
		fp = addr_parse(cr3, fp);
		status = addr_parse(cr3, status);
		s_file *ker_fp = f_open(file, fs_mode, uid, gid);
		if (ker_fp == NULL)
		{
			if (fs_mode == FS_MODE_WRITE)
			{
				fs_create_file(file, uid, gid, 0755);
				ker_fp = f_open(file, fs_mode, uid, gid);
				if (ker_fp == NULL)
				{
					*status = 0;
				}
				else
				{
					*status = 1;
					mmcopy(ker_fp, fp, sizeof(s_file));
					free_mm(ker_fp, sizeof(s_file));
				}
			}
			else
			{
				*status = 0;
			}
		}
		else
		{
			*status = 1;
			mmcopy(ker_fp, fp, sizeof(s_file));
			free_mm(ker_fp, sizeof(s_file));
		}
	}
	//fclose关闭文件
	else if (params[0] == 1)
	{
		s_file *fp = (s_file *) params[1];
		fp = addr_parse(cr3, fp);
		s_file *ker_fp = alloc_mm(sizeof(s_file));
		mmcopy(fp, ker_fp, sizeof(s_file));
		f_close(ker_fp);
	}
	//fwrite写文件
	else if (params[0] == 2)
	{
		s_file *fp = (s_file *) params[1];
		fp = addr_parse(cr3, fp);
		int size = params[2];
		char *data = (char *) params[3];
		data = addr_parse(cr3, data);
		f_write(fp, size, data);
	}
	//fread读文件
	else if (params[0] == 3)
	{
		s_file *fp = (s_file *) params[1];
		fp = addr_parse(cr3, fp);
		int size = params[2];
		char *data = (char *) params[3];
		data = addr_parse(cr3, data);
		f_read(fp, size, data);
	}

	//fgetch取得一个字符
	else if (params[0] == 4)
	{
		s_file *fp = (s_file *) params[1];
		fp = addr_parse(cr3, fp);
		char *data = (char *) params[2];
		data = addr_parse(cr3, data);
		if (f_is_eof(fp))
		{
			*data = 0;
		}
		else
		{
			f_read(fp, 1, data);
		}
	}
	//fgetline取得一行字符
	else if (params[0] == 5)
	{
		s_file *fp = (s_file *) params[1];
		fp = addr_parse(cr3, fp);
		char *data = (char *) params[2];
		data = addr_parse(cr3, data);
		while (!f_is_eof(fp))
		{
			f_read(fp, 1, data);
			if (*data == '\n')
			{
				break;
			}
			data++;
		}
		*data = '\0';
	}
	//fputch写一个字符
	else if (params[0] == 6)
	{
		s_file *fp = (s_file *) params[1];
		fp = addr_parse(cr3, fp);
		char data = (char) params[2];
		f_write(fp, 1, &data);
	}
	//fputline写一行字符
	else if (params[0] == 7)
	{
		s_file *fp = (s_file *) params[1];
		fp = addr_parse(cr3, fp);
		char *data = (char *) params[2];
		data = addr_parse(cr3, data);
		int size = str_len(data);

		f_write(fp, size, data);
		char newline = '\n';
		f_write(fp, 1, &newline);
	}
	//is_eof判断已经是文件尾
	else if (params[0] == 8)
	{
		s_file *fp = (s_file *) params[1];
		fp = addr_parse(cr3, fp);
		int *eof = (int *) params[2];
		eof = addr_parse(cr3, eof);
		if (f_is_eof(fp))
		{
			*eof = 1;
		}
		else
		{
			*eof = 0;
		}
	}
	//fopendir
	else if (params[0] == 9)
	{
		char *path_name = (char *) params[1];
		path_name = addr_parse(cr3, path_name);
		s_file *fs = (s_file *) params[2];
		fs = addr_parse(cr3, fs);
		s_file *fp = f_opendir(path_name);
		if (fp != NULL)
		{
			mmcopy(fp, fs, sizeof(s_file));
		}
		else
		{
			fp->fs.dot = 0;
		}
	}
	//fclosedir
	else if (params[0] == 10)
	{
		s_file *fp = (s_file *) params[1];
		fp = addr_parse(cr3, fp);
		f_closedir(fp);
	}
	//fdirnext
	else if (params[0] == 11)
	{
		s_file *fp = (s_file *) params[1];
		fp = addr_parse(cr3, fp);
		s_file *fnext = fp->next;
		if (fnext != NULL)
		{
			mmcopy(fnext, fp, sizeof(s_file));
		}
		else
		{
			fp->fs.dot = 0;
		}
	}

	set_cr3(cr3);
	set_ds(0xf);
}

/*
 * sys_fs : 文件块数据操作
 *  - int params[0] :
 *  	0 查找指定路径的文件块数据
 * return : void
 */
void sys_fs(int *params)
{
	set_ds(GDT_INDEX_KERNEL_DS);
	set_cr3(PAGE_DIR);
	u32 cr3 = pcb_cur->tss.cr3;
	params = addr_parse(cr3, params);

	//fs_find_path
	if (params[0] == 0)
	{
		char *path = (char *) params[1];
		path = addr_parse(cr3, path);
		u32 uid = (u32) params[2];
		u32 gid = (u32) params[3];
		s_fs **fs = (s_fs **) params[4];
		fs = addr_parse(cr3, fs);
		u32 *dev_id = (u32 *) params[5];
		dev_id = addr_parse(cr3, dev_id);
		int *status = (u32 *) params[6];
		status = addr_parse(cr3, status);
		*status = fs_find_path_by_user(path, uid, gid, dev_id, fs);
	}
	//fs_create_fs
	else if (params[0] == 3)
	{
		char *path = (char *) params[1];
		path = addr_parse(cr3, path);
		char *fs_name = (char *) params[2];
		fs_name = addr_parse(cr3, fs_name);
		u32 uid = (u32) params[3];
		u32 gid = (u32) params[4];
		u32 mode = (u32) params[5];
		int *status = (int *) params[6];
		status = addr_parse(cr3, status);
		u32 s_no = fs_create_fs_path(path, fs_name, uid, gid, mode);
		*status = -1;
		if (s_no != 0)
		{
			*status = 0;
		}
	}
	//fs_del_fs
	else if (params[0] == 4)
	{
		char *path = (char *) params[1];
		path = addr_parse(cr3, path);
		u32 uid = (u32) params[2];
		u32 gid = (u32) params[3];
		int *status = (int *) params[4];
		status = addr_parse(cr3, status);
		*status = fs_del_fs_path(path, uid, gid);
	}

	set_cr3(cr3);
	set_ds(0xf);
}
