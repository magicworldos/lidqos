/*
 * arg.h
 *
 *  Created on: Mar 29, 2015
 *      Author: lidq
 */

#ifndef _INCLUDE_KERNEL_ARG_H_
#define _INCLUDE_KERNEL_ARG_H_

#include <kernel/typedef.h>

typedef u32 va_list;

#define va_init(v,a)				\
	({								\
		v = (va_list)(&a);			\
	})

#define va_arg(v,t)					\
	({								\
		v += 4;						\
		(t)(*((t*)(v)));			\
	})

#endif /* INCLUDE_KERNEL_ARG_H_ */
