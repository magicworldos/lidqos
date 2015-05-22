/*
 * unistd.c
 *
 *  Created on: May 22, 2015
 *      Author: lidq
 */

#include <shell/unistd.h>

/*
 * 等待毫秒
 */
void msleep(int ms)
{
	//定义系统调用参数表
	int params[2];
	//调用索引为2代表是等待
	params[0] = 2;
	//毫秒数
	params[1] = ms;
	//系统中断，将参数表的地址传入中断服务程序
	__asm__ volatile("int	$0x80" :: "a"(params));
}

/*
 * 等待秒
 */
void sleep(int s)
{
	// 1s == 1000ms
	msleep(s * 1000);
}
