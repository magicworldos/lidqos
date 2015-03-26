#ifndef _BOOT_S_
#define _BOOT_S_

//16位操作数和16位寻址模式
.code16

#include <boot/boot.h>

//开始过程
.global		_start
//数据段
.section	.data

//代码段
.section	.text

//开始
_start:
	//设定段寄存器为0x7c00
	movw	%cs, %ax
	movw	%ax, %ds
	movw	%ax, %es
	movw	%ax, %ss
	//清空%sp
	xorw	%sp, %sp

	//跳转到0x7c00运程程序
	ljmp	$_SEG_BOOT, $_setup

_setup:

	//将0x7c00处的boot程序copy到_SEG_MAIN处
	calll	_copy_boot

	//跳转到0x90000处来执行程序
	ljmp	$_SEG_MAIN, $_boot

//0x90000
_boot:
	//将0x9c00处的kernel程序copy到0x0处
	calll	_copy_kernel

	//设置data段ds、es和ss:sp
	movw	$_SEG_MAIN, %ax
	movw	%ax, %ds
	movw	%ax, %es
	movw	%ax, %fs
	movw	%ax, %gs
	movw	%ax, %ss
	xorw	%sp, %sp

	//测试，如果显示字符a说明程序执行正常
	//calll	_display_ASCII

	//调用_SEG_MAIN处的main()
	calll	main

_loop:
	jmp	_loop

//将0x7c00处的boot程序copy到0x90000处
_copy_boot:
	//保存现场
	pushw	%ax
	pushw	%bx
	pushw	%cx
	pushw	%dx
	pushw	%es

	//将es和di设置为0x90000
	xorw	%ax, %ax
	movw	$_SEG_MAIN, %ax
	movw	%ax, %es
	xorw	%ax, %ax
	movw	%ax, %di

	//将ds和si设置为0x7c00
	xorw	%ax, %ax
	movw	$_SEG_BOOT, %ax
	movw	%ax, %ds
	xorw	%ax, %ax
	movw	%ax, %si

	//将cx设置成启动程序大小
	movw	$_SEG_BOOT_SIZE, %cx
	cld
	//循环拷贝启动程序到0x90000
	rep 	movsb %ds:(%si), %es:(%di)

	//恢复现场
	popw	%es
	popw	%dx
	popw	%cx
	popw	%bx
	popw	%ax

	retl
_copy_boot_end:	nop

//已经被拷贝到了0x90000处的过程
_copy_kernel:
	//保存现场
	pushw	%ax
	pushw	%bx
	pushw	%cx
	pushw	%dx
	pushw	%es

	//将es和di设置为0x0
	xorw	%ax, %ax
	movw	%ax, %es
	xorw	%ax, %ax
	movw	%ax, %di

	//将ds和si设置为0x9c00
	xorw	%ax, %ax
	movw	$_SEG_KERNEL, %ax
	movw	%ax, %ds
	xorw	%ax, %ax
	movw	%ax, %si

	//将cx设置成kernel程序大小
	movl	$_KERNEL_SIZE, %ecx
	cld
	//循环拷贝kernel程序到0x0
	rep 	movsb %ds:(%si), %es:(%di)

	//恢复现场
	popw	%es
	popw	%dx
	popw	%cx
	popw	%bx
	popw	%ax

	retl
_copy_kernel_end:	nop

_display_ASCII:
	//保存现场
	pushw	%ax
	pushw	%bx
	pushw	%cx
	pushw	%dx
	pushw	%es

	movw	$0xb800, %ax
	movw	%ax, %es
	xorw	%ax, %ax
	movw	%ax, %di
	movw	$0x0761, %ax
	movw	%ax, %es:(%di)

	//恢复现场
	popw	%es
	popw	%dx
	popw	%cx
	popw	%bx
	popw	%ax

_display_ASCII_end: nop

	//占位，从此行开始到0x1fe止匀为0x90也就是nop
	.org	0x1fe,	0x90
	//在0x1ff终止符0xaa55
	.word	0xaa55
	//一共0x200个字节

#endif
