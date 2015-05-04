/*
 * page.h
 *
 *  Created on: Apr 20, 2015
 *      Author: lidq
 */

#ifndef _INCLUDE_KERNEL_PAGE_H_
#define _INCLUDE_KERNEL_PAGE_H_

#include <kernel/typedef.h>
#include <kernel/io.h>
#include <kernel/alloc.h>
#include <kernel/printf.h>

#define PAGE_DIR			(0x700000)
#define PAGE_TABLE			(PAGE_DIR + MM_PAGE_SIZE)

void install_page();

void page_error(u32 error_code);

#endif /* INCLUDE_KERNEL_PAGE_H_ */