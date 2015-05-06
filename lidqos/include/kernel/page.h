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
#include <kernel/process.h>
#include <kernel/hd.h>
#include <kernel/printf.h>

#define PAGE_DIR			(0x700000)
#define PAGE_TABLE			(PAGE_DIR + MM_PAGE_SIZE)

void install_page();

void page_error(u32 pid, u32 error_code);

int alloc_page_no(u32 pid, u32 page_no);

int page_swap_out(u32 page_no);

int page_swap_in(u32 page_no, u32 sec_no, u32 pid);

#endif /* INCLUDE_KERNEL_PAGE_H_ */
