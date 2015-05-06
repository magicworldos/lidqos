/*
 * kernel.c
 *
 *  Created on: Mar 26, 2015
 *      Author: lidq
 */

#include <kernel/kernel.h>

//内核启动程序入口
int start_kernel(int argc, char **args)
{
	printf("Welcome to LidqOS.\n");

	//安装内存申请模块
	install_alloc();

	//安装内存页面使用状态
	install_used_map();

	//安装GDT全局描述符
	install_gdt();

	//安装ISR中断服务程序
	install_idt();

	//安装8259A
	install_pic();

	//安装时钟中断
	install_timer();

	//安装键盘中断
	install_kb();

	//安装多任务
	install_process();

	//安装内在分页
	install_page();

	//安装交换空间
	install_swap();

	//开中断，在进入保护模式前已经关闭了中断这时需要将其打开
	sti();

	for (;;)
	{
	}

	return 0;
}
