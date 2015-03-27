#ifndef _BOOT_S_
#define _BOOT_S_

//16位操作数和16位寻址模式
.code16

#include <boot/boot.h>

//开始过程
.global		_start, _to_the_protect_mode
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

_boot_end:	nop

//将0x7c00处的boot程序copy到0x90000处
_copy_boot:
	//保存现场
	pushw	%ax
	pushw	%bx
	pushw	%cx
	pushw	%dx
	pushw	%es
	pushw	%ds
	pushw	%si
	pushw	%di

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
	popw	%di
	popw	%si
	popw	%ds
	popw	%es
	popw	%dx
	popw	%cx
	popw	%bx
	popw	%ax

	retl
_copy_boot_end:	nop

_display_ASCII:
	//保存现场
	pushw	%ax
	pushw	%bx
	pushw	%cx
	pushw	%dx
	pushw	%es
	pushw	%ds
	pushw	%si
	pushw	%di

	movw	$0xb800, %ax
	movw	%ax, %es
	xorw	%ax, %ax
	movw	%ax, %di
	movw	$0x0761, %ax
	movw	%ax, %es:(%di)

	//恢复现场
	popw	%di
	popw	%si
	popw	%ds
	popw	%es
	popw	%dx
	popw	%cx
	popw	%bx
	popw	%ax

	retl

_display_ASCII_end: nop

//跳转到保护模式
_to_the_protect_mode:

	//保存现场
	pushl	%ebp
	movl	%esp,	%ebp

	//载入gdt全局描述符
	lgdt	gdtp

	//打开保护模式，将cr0的0位置成1
	movl    %cr0,	%eax
    orl     $0x1,	%eax
    movl    %eax,	%cr0

    //将0x9c00处的kernel程序copy到0x0处
	movw	$_GDT_IND_KERNEL_DATA, %ax
	movw	%ax,	%ds
	movw	%ax,	%es

	movl	$_SEG_KERNEL_PH, %esi
	movl	$0x0,	%edi

	//将%ecx寄存器置成内核程序大小
	movl	$_KERNEL_SIZE,	%ecx

_copy_kernel2:
	movw	%ds:(%esi), %ax
	movw	%ax, %es:(%edi)

	//每次加2
	add 	$0x2, %esi
	add		$0x2, %edi
	sub		$0x2, %ecx

	cmp		$0x0, %ecx
	jne		_copy_kernel2
	jmp		_copy_kernel2_end
_copy_kernel2_end: nop

	//处理所有寄存器，为跳转到保护模式做准备
	movw	$_GDT_IND_KERNEL_DATA,	%ax
	movl	$_SEG_KERNEL_DATA_OFFSET,	%ebx
	//设定全局选择子
	movw	%ax,	%ds
	movw	%ax,	%es
	movw	%ax,	%fs
	movw	%ax,	%gs
	movw	%ax,	%ss

	//设定相对内存地址
	xorl	%eax,	%eax
	movl	%eax,	%edi
	movl	%eax,	%esi

	//设定堆栈地址
	movl	%ebx,	%esp
	movl	%ebx,	%ebp

//跳转到(0x000000000)处，不再返回这里
//内核实际地址在_SEG_KERNEL_OFFSET
_ljmp:
	.byte	0xea
_ljmp_offset:
	.word	_SEG_KERNEL_OFFSET
_ljmp_section:
	.word	_GDT_IND_KERNEL

	//恢复现场
	popl	%ebp
	retl
_to_the_protect_mode_end: nop

	//占位，从此行开始到0x1fe止匀为0x90也就是nop
	.org	0x1fe,	0x90
	//在0x1ff终止符0xaa55
	.word	0xaa55
	//一共0x200个字节

#endif
