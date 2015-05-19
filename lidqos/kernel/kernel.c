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

	//安装内核程序
	//install_kernel();

	//安装内存申请模块
	install_alloc();

	//安装内存页面使用状态
	install_used_map();

	//安装内存分页
	install_page();

	//安装GDT全局描述符
	install_gdt();

	//安装ISR中断服务程序
	install_idt();

	//安装8259A
	install_pic();

	//安装时钟中断
	install_timer();

	//安装hda硬盘中断
	install_hda();

	//安装键盘中断
	install_kb();

	//安装文件系统
	install_fs_system();

	//安装交换空间
	install_swap();

	//安装多任务
	install_process();

	//安装系统首行任务
	install_system();

	//开中断，在进入保护模式前已经关闭了中断这时需要将其打开
	sti();

	kernel_running();

	return 0;
}

void install_kernel()
{

	//载入内核程序
	char *data = (char *) KERNEL_INS_TEMP_ADDR;
	for (int i = 0; i < KERNEL_INS_SECTOR_COUNT; ++i)
	{
		osiso_read_block_phy(ATA_BUS_PRIMARY, ATA_DRIVE_SLAVE, KERNEL_INS_START_SECTOR + i, (char *) (data + 0x800 * i));
	}
	__asm__ volatile("									\t\n"
			"pushal										\t\n"
			"pushfl										\t\n"

			"movl " KERNEL_INS_TEMP_ADDR_STR ", %eax	\t\n"
			"movl %eax, %edi							\t\n"
			"movl " KERNEL_INS_ADDR_STR ", %eax			\t\n"
			"movl %eax, %esi							\n\t"

			"movl " KERNEL_INS_SIZE_STR ", %ecx			\t\n"
			"_cp1: 										\t\n"
			"movw (%edi), %ax							\t\n"
			"movw %ax, (%esi) 							\t\n"

			"add $0x2, %edi								\t\n"
			"add $0x2, %esi								\t\n"
			"sub $0x2, %ecx								\t\n"
			"cmp $0x0, %ecx								\t\n"

			"jne _cp1									\t\n"

			"popfl										\t\n"
			"popal										\t\n"
			"");
}

void kernel_running()
{
	RUNNING_WITH_FOR
}
