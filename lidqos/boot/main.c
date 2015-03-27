/*
 * main.c
 *
 *  Created on: Mar 26, 2015
 *      Author: lidq
 */

#include <boot/code16.h>
#include <boot/main.h>

//全局描述符表个数
#define GDT_MAX_SIZE (3)

//全局描述符表
s_gdt gdts[GDT_MAX_SIZE];

//全局描述符
s_gdtp gdtp;

int main(int argc, char **args)
{
	//关中断
	cli();

	//打开A20，启用CPU的32根内存寻址线，可进行4GB内存寻址
	enable_a20();

	//设置全局描述符
	set_gdt();

	//跳转到保护模式，不再返回，直接启动内核程序
	to_protect_mode();

	return 0;
}

/*
 * addr_to_gdt : 将物理地址转为gdt描述地址，并存放到gdt全局描述符当中
 *  - u32 addr : 32位物理地址
 *  - s_gdt	*gdt : gdt全局描述符
 *  - u8 cs_ds : 0为代码段，1为数据段
 * return : void
 */
void addr_to_gdt(u32 addr, s_gdt *gdt, u8 cs_ds)
{
	//最大尺寸低16位
	gdt->limit = 0xffff;
	//基地址低16位
	gdt->baseaddr = addr & 0xffff;
	//基地址中8位
	gdt->baseaddr2 = (addr >> 16) & 0xff;
	//如果是代码段
	if (cs_ds == 0)
	{
		//代码段描述
		gdt->p_dpl_type_a = 0x9a;
	}
	//如果是数据段
	else
	{
		//数据段描述
		gdt->p_dpl_type_a = 0x92;
	}
	//相关标识和最大尺寸高位
	gdt->uxdg_limit2 = 0xcf;
	//基地址高8位
	gdt->baseaddr3 = (addr >> 24) & 0xff;
}

/*
 * set_gdt : 设置全局gdt描述符，其中包括默认地址、内核地址、显存地址等
 * return : void
 */
void set_gdt()
{
	//默认地址
	u32 addr = 0x0;

	//默认空描述符0x0
	gdts[0].gdt = 0x0;
	gdts[0].gdt2 = 0x0;

	//设置kernel的全局描述符0x8
	addr = 0x0;
	addr_to_gdt(addr, &gdts[1], 0);

	//设置kernel data的全局描述符0x10
	addr = 0x0;
	addr_to_gdt(addr, &gdts[2], 1);

	//设置gdt描述符
	//gdt总数-1
	gdtp.gdt_lenth = sizeof(s_gdt) * GDT_MAX_SIZE - 1;
	//gdt全局描述符的高16位地址
	gdtp.gdt_addr2 = ds() * 0x10 >> 16;
	//gdt全局描述符地址低16位
	gdtp.gdt_addr = (u32) gdts;
}

/*
 * enable_a20 : 打开A20
 * return : void
 */
void enable_a20()
{
	u8 port_a;
	//从0x92端口读入数据
	port_a = inb_p(0x92);
	//打开A20
	port_a |= 0x02;
	//不重置电脑
	port_a &= ~0x01;
	//向0x92输出设定后的值
	outb_p(port_a, 0x92);
}

/*
 * to_protect_mode : 跳转到保护模式，不再返回，直接启动内核程序
 * return : void
 */
void to_protect_mode()
{
	//跳转到保护模式，不再返回，直接启动内核程序
	_to_the_protect_mode();
}
