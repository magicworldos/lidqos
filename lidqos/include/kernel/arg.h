/*
 * arg.h
 *
 *  Created on: Mar 29, 2015
 *      Author: lidq
 */

#ifndef _INCLUDE_KERNEL_ARG_H_
#define _INCLUDE_KERNEL_ARG_H_

#include <kernel/typedef.h>

//定义动态参数地址号
typedef u32 va_list;

/***
 * 初始化动态参数地址
 * v: 动态参数地址号
 * a: 前一个参数变量
 */
#define va_init(v, a)				\
	({								\
		v = (va_list)(&a);			\
	})

/***
 * 取得下一个参数的值
 * v: 动态参数地址号
 * t: 下一个参数的类型
 * return: 返回下一个参数的值
 */
#define va_arg(v, t)				\
	({								\
		v += 4;						\
		(t)(*((t*)(v)));			\
	})

#endif /* INCLUDE_KERNEL_ARG_H_ */
