/*
 * sys_var.h
 *
 *  Created on: Dec 1, 2014
 *      Author: lidq
 */

#ifndef _SYS_VAR_H_
#define _SYS_VAR_H_

#include <kernel/typedef.h>

//任务状态：运行
#define T_S_RUNNING		(0)
//任务状态：就绪
#define T_S_READY		(1)
//任务状态：阻塞
#define T_S_WAIT		(2)
//任务状态：停止
#define T_S_STOP		(3)

//进程栈页面数
#define P_STACK_P_NUM	(0x1)
//进程栈大小
#define P_STACK_SIZE	(0x1000)
//进程0级栈页面数
#define P_STACK0_P_NUM	(0x1)
//进程0级栈大小
#define P_STACK0_SIZE	(0x1000)
//进程页目录数
#define P_PAGE_DIR_NUM	(0x1)
//进程页表数
#define P_PAGE_TBL_NUM	(0x400)
//时钟周期10ms
#define CLOCK_TIMER		(10)

//制表符使用4个空格
#define VIDEO_TAB_SIZE			(4)
//显示字符串缓冲区大小
#define PRINTF_BUFF_SIZE		(1024)

//显示格式中%f为float %F为double
//显示格式中%e为float %E为double
#define LF_TYPE_FLOAT 			(1)
#define LF_TYPE_DOUBLE 			(0)
//浮点精度
#define ZERO_FLOAT 				(0.00001)
//双精度浮点数精度
#define ZERO_DOUBLE 			(0.00000000000000001)
//浮点数小数位
#define FLOAT_SIZE 				(6)
//双精度浮点数小数位
#define DOUBLE_SIZE 			(18)

//fpu需要内存存储字节数
#define FPU_SIZE 				(0x200)

//环境变量最大长度
#define SHELL_PATH_LEN				(0x1000)

//命令最大长度
#define SHELL_CMD_LEN				(0x200)
//退出命令
#define SHELL_CMD_EXIT				("exit")

//用户信息，用户名长度
#define USER_U_LEN					(0x200)
//用户信息，密码长度
#define USER_P_LEN					(0x200)
//用户信息，主目录长度
#define USER_H_LEN					(0x200)
//用户信息，shell长度
#define USER_S_LEN					(0x200)

//登录状态，未登录
#define SESSION_STATUS_NOLOGIN		(0)
//登录状态，已登录
#define SESSION_STATUS_LOGIN		(1)

//tss数据结构
typedef struct tss_s
{
	u32 back_link;
	u32 esp0, ss0;
	u32 esp1, ss1;
	u32 esp2, ss2;
	u32 cr3;
	u32 eip;
	u32 eflags;
	u32 eax, ecx, edx, ebx;
	u32 esp, ebp;
	u32 esi, edi;
	u32 es, cs, ss, ds, fs, gs;
	u32 ldt;
	u32 trace_bitmap;
} s_tss;

typedef struct shell_path_s
{
	char *path;
	struct shell_path_s *next;
} s_path;

//session数据结构
typedef struct session_s
{
	//用户ID
	int uid;
	//用户组ID
	int gid;
	//用户名
	char *username;
	//主目录
	char *home;
	//工作目录
	char *current_path;
	//当前目录
	char *current_folder_name;

	//环境变量
	s_path *PATH;
	//状态
	int status;

} s_session;

typedef struct alloc_list_s
{
	void* addr;
	int size;
	struct alloc_list_s *next;

} s_alloc_list;

/*
 * 进程控制块
 */
typedef struct process_control_block
{
	//进程号
	u32 process_id;
	//任务描述段
	s_tss tss;
	//代码段和数据段的局部描述符
	s_gdt ldt[2];
	//程序地址
	void *run;
	//start_main函数地址偏移
	u32 run_offset;
	//程序大小
	u32 run_size;
	//页目录
	void *page_dir;
	//页表
	void *page_tbl;
	//程序栈
	void *stack;
	//程序0级栈
	void *stack0;
	//等待毫秒数（剩余）
	int sleep_ms;
	//父进程/线程
	void *parent;
	//子进程/线程
	void *children;
	//是否需要使用fpu
	int is_need_fpu;
	//浮点寄存器数据保存区
	u8 *fpu_data;
	//用户登录信息
	s_session session;
	//内存申请表
	void *alloc_list;
	//shell运行pcb的同步信号量
	s_sem sem_shell[2];
} s_pcb;

typedef struct s_hda_rw
{
	//扇区号
	u32 lba;
	//读写命令
	u8 com;
	//数据缓冲区
	u8* buff;
	//操作状态
	int* status;
	//链表的下一个节点
	struct s_hda_rw *next;
} s_hda_rw;

typedef struct s_partition_table
{
	//设备号
	u8 device;
	//是否可以启动
	u8 boot;
	//分区ID号
	u8 id;
	//分区类型
	u8 type;
	//位图开启扇区号
	u32 bitmap_start;
	//数据开启扇区号
	u32 start;
	//分区扇区数
	u32 size;
} s_pt;

typedef struct
{
	s_pt pts[0x20];
	int pt_count;

} s_sys_var;

void install_sys_var();

void install_pts(void *pts);

#endif /* SYS_VAR_H_ */
