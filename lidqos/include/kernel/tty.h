/*
 * tty.h
 *
 *  Created on: Jun 4, 2015
 *      Author: lidq
 */

#ifndef _INCLUDE_KERNEL_TTY_H_
#define _INCLUDE_KERNEL_TTY_H_

#include <kernel/typedef.h>
#include <kernel/sys_var.h>
#include <kernel/alloc.h>
#include <kernel/printf.h>

void install_tty();

void switch_tty(int tty_id);

#endif /* INCLUDE_KERNEL_TTY_H_ */
