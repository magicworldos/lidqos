#ifndef _BOOT_S_
#define _BOOT_S_

//16位操作数和16位寻址模式
.code16
//开始过程
.global		_start
//数据段
.section	.data

//代码段
.section	.text

//开始
_start:

	//读到es:bx为0x7e00处
	movw	$0x7e0,	%ax
	movw	%ax, %es
	xorw	%bx, %bx

	//0x2为读扇区
	movb	$0x2, %ah
	//读入扇区数
	movb	$0x11, %al
	//磁道号
	movb	$0x0, %ch
	//扇区号
	movb	$0x2, %cl
	//柱面号
	movb	$0x0, %dh
	//驱动器号，软驱A为0，软驱B为1
	movb	$0x0, %dl
	//调用BISO磁盘中断
	int		$0x13

	//将0x7c00处的boot程序copy到0x90000处
	calll	_load_boot

	//跳转到0x90000处来执行程序
	ljmp	$0x9000, $_copy_kernel

//将0x7c00处的boot程序copy到0x90000处
_load_boot:
	//保存现场
	pushw	%ax
	pushw	%bx
	pushw	%cx
	pushw	%dx
	pushw	%es

	//将es和di设置为0x90000
	xorw	%ax, %ax
	movw	$0x9000, %ax
	movw	%ax, %es
	xorw	%ax, %ax
	movw	%ax, %di

	//将ds和si设置为0x7c00
	xorw	%ax, %ax
	movw	$0x7c0, %ax
	movw	%ax, %ds
	xorw	%ax, %ax
	movw	%ax, %si

	//将cx设置成启动程序大小
	movw	$0x2000, %cx
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
_load_boot_end:	nop

//已经被拷贝到了0x90000处的过程
_copy_kernel:
	//将es和di设置为0x0
	xorw	%ax, %ax
	movw	%ax, %es
	xorw	%ax, %ax
	movw	%ax, %di

	//将ds和si设置为0x9c00
	xorw	%ax, %ax
	movw	$0x9c0, %ax
	movw	%ax, %ds
	xorw	%ax, %ax
	movw	%ax, %si

	//将cx设置成kernel程序大小
	movw	$0x200, %cx
	cld
	//循环拷贝kernel程序到0x0
	rep 	movsb %ds:(%si), %es:(%di)

//永不停歇的循环
_loop:
	jmp		_loop

_copy_kernel_end:	nop

	//占位，从此行开始到0x1fe止匀为0x90也就是nop
	.org	0x1fe,	0x90
	//在0x1ff终止符0xaa55
	.word	0xaa55
	//一共0x200个字节

#endif
