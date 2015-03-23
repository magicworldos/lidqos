#ifndef _BOOT_S_
#define _BOOT_S_

//16位操作数和16位寻址模式
.code16
//开始过程
.global _start
//数据段
.section .data
//代码段
.section .text

//开始
_start:
	jmp	_start
//占位，从此行开始到0x1fe止匀为0x90也就是nop
.org	0x1fe,	0x90
//在0x1ff终止符0xaa55
.word	0xaa55
//一共0x200个字节

#endif
