/*
 * kernel.h
 *
 *  Created on: Mar 26, 2015
 *      Author: lidq
 */

#ifndef _INCLUDE_KERNEL_KERNEL_H_
#define _INCLUDE_KERNEL_KERNEL_H_

#include <kernel/typedef.h>
#include <kernel/printf.h>
#include <kernel/alloc.h>
#include <kernel/page.h>
#include <kernel/mm.h>
#include <kernel/process.h>
#include <kernel/page.h>

int start_kernel(int argc, char **args);

#endif /* INCLUDE_KERNEL_KERNEL_H_ */
