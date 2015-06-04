/*
 * int.S
 * Copyright (C) Feb 10, 2014 by lidq
 *
 * 中断程序
 *  - 采用汇编过程编写的外中断程序和系统中断程序
 */

 .code32

//全局过程函数
.global _int_default, _isr

//数据段
.section .data

//中断响应函数
_isr:
	.long	_int_0x00, _int_0x01, _int_0x02, _int_0x03, _int_0x04, _int_0x05, _int_0x06, _int_0x07
	.long	_int_0x08, _int_0x09, _int_0x0a, _int_0x0b, _int_0x0c, _int_0x0d, _int_0x0e, _int_0x0f
	.long	_int_0x10, _int_0x11, _int_0x12, _int_0x13, _int_0x14, _int_0x15, _int_0x16, _int_0x17
	.long	_int_0x18, _int_0x19, _int_0x1a, _int_0x1b, _int_0x1c, _int_0x1d, _int_0x1e, _int_0x1f
	.long	_int_0x20, _int_0x21, _int_0x22, _int_0x23, _int_0x24, _int_0x25, _int_0x26, _int_0x27
	.long	_int_0x28, _int_0x29, _int_0x2a, _int_0x2b, _int_0x2c, _int_0x2d, _int_0x2e, _int_0x2f

	//system call
	.long	_int_0x80, _int_0x81, _int_0x82, _int_0x83, _int_0x84, _int_0x85, _int_0x86, _int_0x87
	.long	_int_0x88, _int_0x89, _int_0x8a, _int_0x8b, _int_0x8c, _int_0x8d, _int_0x8e, _int_0x8f
	.long	_int_0x90, _int_0x91, _int_0x92, _int_0x93, _int_0x94, _int_0x95, _int_0x96, _int_0x97
	.long	_int_0x98, _int_0x99, _int_0x9a, _int_0x9b, _int_0x9c, _int_0x9d, _int_0x9e, _int_0x9f

//代码段
.section .text

//默认中断程序
_int_default:
	iret

//除零错
_int_0x00:
	cli
	pushal
	pushfl
	//C函数中参数和变量所用的栈
	push	%ebp
	mov		%esp, %ebp
	//调用C函数来处理这个异常
	call	int_div_error
	leave
	popfl
	popal
	sti
	iret

//调试异常
_int_0x01:
	cli
	pushal
	pushfl
	//C函数中参数和变量所用的栈
	push	%ebp
	mov		%esp, %ebp
	//调用C函数来处理这个异常
	call	int_debug_error
	leave
	popfl
	popal
	sti
	iret

//不可屏蔽中断
_int_0x02:
	cli
	pushal
	pushfl
	//C函数中参数和变量所用的栈
	push	%ebp
	mov		%esp, %ebp
	//调用C函数来处理这个异常
	call	int_nmi
	leave
	popfl
	popal
	sti
	iret

//断电
_int_0x03:
	cli
	pushal
	pushfl
	//C函数中参数和变量所用的栈
	push	%ebp
	mov		%esp, %ebp
	//调用C函数来处理这个异常
	call	int_power_down
	leave
	popfl
	popal
	sti
	iret

//上溢出
_int_0x04:
	cli
	pushal
	pushfl
	//C函数中参数和变量所用的栈
	push	%ebp
	mov		%esp, %ebp
	//调用C函数来处理这个异常
	call	int_bound_out
	leave
	popfl
	popal
	sti
	iret

//边界检查
_int_0x05:
	cli
	pushal
	pushfl
	//C函数中参数和变量所用的栈
	push	%ebp
	mov		%esp, %ebp
	//调用C函数来处理这个异常
	call	int_bound_check
	leave
	popfl
	popal
	sti
	iret

//无效操作码
_int_0x06:
	cli
	pushal
	pushfl
	//C函数中参数和变量所用的栈
	push	%ebp
	mov		%esp, %ebp
	//调用C函数来处理这个异常
	call	int_invalid_opcode
	leave
	popfl
	popal
	sti
	iret

//没有浮点运算器
_int_0x07:
	cli
	pushal
	pushfl
	//C函数中参数和变量所用的栈
	push	%ebp
	mov		%esp, %ebp
	//调用C函数来处理这个异常
	call 	int_no_fpu
	leave
	popfl
	popal
	sti
	iret

//双重错误
_int_0x08:
	cli
	pushal
	pushfl
	//C函数中参数和变量所用的栈
	push	%ebp
	mov		%esp, %ebp
	//调用C函数来处理这个异常
	call 	int_double_error
	leave
	popfl
	popal
	sti
	iret

//浮点运算器段超限
_int_0x09:
	cli
	pushal
	pushfl
	//C函数中参数和变量所用的栈
	push	%ebp
	mov		%esp, %ebp
	//调用C函数来处理这个异常
	call	int_fpu_out
	leave
	popfl
	popal
	sti
	iret

//无效TSS
_int_0x0a:
	cli
	pushal
	pushfl
	//C函数中参数和变量所用的栈
	push	%ebp
	mov		%esp, %ebp
	//调用C函数来处理这个异常
	call	int_tss_error
	leave
	popfl
	popal
	sti
	iret

//无效段
_int_0x0b:
	cli
	pushal
	pushfl
	//C函数中参数和变量所用的栈
	push	%ebp
	mov		%esp, %ebp
	//调用C函数来处理这个异常
	call	int_section_error
	leave
	popfl
	popal
	sti
	iret

//无效栈
_int_0x0c:
	cli
	pushal
	pushfl
	//C函数中参数和变量所用的栈
	push	%ebp
	mov		%esp, %ebp
	//调用C函数来处理这个异常
	call	int_stack_error
	leave
	popfl
	popal
	sti
	iret

//一般保护错误
_int_0x0d:
	cli
	pushal
	pushfl
	//C函数中参数和变量所用的栈
	push	%ebp
	mov		%esp, %ebp
	//调用C函数来处理这个异常
	call	int_protection_error
	leave
	popfl
	popal
	sti
	iret

//页错误
_int_0x0e:

	/*
	 * 弹出因缺页错误而被压栈的内容
	 * 程序运行时使用%eax作为变量赋值的临时存放区，这时如果采用popl %eax的方式来处理
	 * 当此中断执行完毕iret执行之后%eax的值并没有被还原为原来的值，所以程序不正确
	 */
	cli
	popl	%edx
	pushal
	pushfl

	//C函数中参数和变量所用的栈
	push	%ebp
	mov		%esp, %ebp
	sub		$0x28, %esp
    movl	%edx, (%esp)
	//调用C函数来处理这个异常
	call	int_page_error
	leave
	popfl
	popal
	sti
	iret

//保留
_int_0x0f:
	iret

//浮点运算器错误
_int_0x10:
	cli
	pushal
	pushfl
	//C函数中参数和变量所用的栈
	push	%ebp
	mov		%esp, %ebp
	//调用C函数来处理这个异常
	call	int_fpu_error
	leave
	popfl
	popal
	sti
	iret

//保留
_int_0x11:
	iret

//保留
_int_0x12:
	iret

//保留
_int_0x13:
	iret

//保留
_int_0x14:
	iret

//保留
_int_0x15:
	iret

//保留
_int_0x16:
	iret

//保留
_int_0x17:
	iret

//保留
_int_0x18:
	iret

//保留
_int_0x19:
	iret

//保留
_int_0x1a:
	iret

//保留
_int_0x1b:
	iret

//保留
_int_0x1c:
	iret

_int_0x1d:
	iret

//保留
_int_0x1e:
	iret

//保留
_int_0x1f:
	iret

//时钟中断
_int_0x20:
	cli
	pushal
	pushfl
	//调用时钟中断处理函数
	call int_timer
	popfl
	popal
	sti
	iret

//键盘中断
_int_0x21:
	cli
	pushal
	pushfl
	//调用键盘中断处理函数
	call int_keyboard
	popfl
	popal
	sti
	iret

_int_0x22:
	iret

_int_0x23:
	iret

_int_0x24:
	iret

_int_0x25:
	iret

_int_0x26:
	iret

_int_0x27:
	iret

_int_0x28:
	iret

_int_0x29:
	iret

_int_0x2a:
	iret

_int_0x2b:
	iret

_int_0x2c:
	iret

_int_0x2d:
	iret

_int_0x2e:
	iret

_int_0x2f:
	iret

_int_0x80:
	cli
	pushal
	pushfl
	push	%ebp
	mov 	%esp, %ebp
	sub		$0x4, %esp
    movl	%eax, (%esp)
	call	sys_process
	leave
	popfl
	popal
	sti
	iret

_int_0x81:
	cli
	pushal
	pushfl
	push	%ebp
	mov 	%esp, %ebp
	sub		$0x4, %esp
    movl	%eax, (%esp)
	call	sys_semaphore
	leave
	sti
	popfl
	popal
	iret

_int_0x82:
	cli
	pushal
	pushfl
	push	%ebp
	mov 	%esp, %ebp
	sub		$0x4, %esp
    movl	%eax, (%esp)
	call	sys_stdio
	leave
	popfl
	popal
	sti
	iret

_int_0x83:
	cli
	pushal
	pushfl
	push	%ebp
	mov 	%esp, %ebp
	sub		$0x4, %esp
    movl	%eax, (%esp)
	call	sys_stdlib
	leave
	popfl
	popal
	sti
	iret

_int_0x84:
	cli
	pushal
	pushfl
	push	%ebp
	mov 	%esp, %ebp
	sub		$0x4, %esp
    movl	%eax, (%esp)
	call	sys_hd_rw
	leave
	popfl
	popal
	sti
	iret

_int_0x85:
	cli
	pushal
	pushfl
	push	%ebp
	mov 	%esp, %ebp
	sub		$0x4, %esp
    movl	%eax, (%esp)
	call	sys_pts
	leave
	popfl
	popal
	sti
	iret

_int_0x86:
	cli
	pushal
	pushfl
	push   %ebp
	mov    %esp,%ebp
	sub		$0x4, %esp
    movl	%eax, (%esp)
	call    sys_file
	leave
	popfl
	popal
	sti
	iret

_int_0x87:
	cli
	pushal
	pushfl
	push   %ebp
	mov    %esp,%ebp
	sub		$0x4, %esp
    movl	%eax, (%esp)
	call    sys_fs
	leave
	popfl
	popal
	sti
	iret

_int_0x88:
	push	%ebp
	mov 	%esp, %ebp

	leave
	iret

_int_0x89:
	push	%ebp
	mov 	%esp, %ebp

	leave
	iret

_int_0x8a:
	push	%ebp
	mov 	%esp, %ebp

	leave
	iret

_int_0x8b:
	push	%ebp
	mov 	%esp, %ebp

	leave
	iret

_int_0x8c:
	push	%ebp
	mov 	%esp, %ebp

	leave
	iret

_int_0x8d:
	push	%ebp
	mov 	%esp, %ebp

	leave
	iret

_int_0x8e:
	push	%ebp
	mov 	%esp, %ebp

	leave
	iret

_int_0x8f:
	push	%ebp
	mov 	%esp, %ebp

	leave
	iret

_int_0x90:
	push	%ebp
	mov 	%esp, %ebp

	leave
	iret

_int_0x91:
	push	%ebp
	mov 	%esp, %ebp

	leave
	iret

_int_0x92:
	push	%ebp
	mov 	%esp, %ebp

	leave
	iret

_int_0x93:
	push	%ebp
	mov 	%esp, %ebp

	leave
	iret

_int_0x94:
	push	%ebp
	mov 	%esp, %ebp

	leave
	iret

_int_0x95:
	push	%ebp
	mov 	%esp, %ebp

	leave
	iret

_int_0x96:
	push	%ebp
	mov 	%esp, %ebp

	leave
	iret

_int_0x97:
	push	%ebp
	mov 	%esp, %ebp

	leave
	iret

_int_0x98:
	push	%ebp
	mov 	%esp, %ebp

	leave
	iret

_int_0x99:
	push	%ebp
	mov 	%esp, %ebp

	leave
	iret

_int_0x9a:
	push	%ebp
	mov 	%esp, %ebp

	leave
	iret

_int_0x9b:
	push	%ebp
	mov 	%esp, %ebp

	leave
	iret

_int_0x9c:
	push	%ebp
	mov 	%esp, %ebp

	leave
	iret

_int_0x9d:
	push	%ebp
	mov 	%esp, %ebp

	leave
	iret

_int_0x9e:
	push	%ebp
	mov 	%esp, %ebp

	leave
	iret

_int_0x9f:
	push	%ebp
	mov 	%esp, %ebp

	leave
	iret
