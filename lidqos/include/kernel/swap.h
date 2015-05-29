/*
 * swap.h
 *
 *  Created on: May 16, 2015
 *      Author: lidq
 */

#ifndef _MM_SWAP_H_
#define _MM_SWAP_H_

#include <kernel/sys_var.h>
#include <kernel/hd.h>

void install_swap(s_pt *pts);

u32 swap_alloc_sec();

void swap_free_sec(u32 sec_no);

u32 swap_alloc_page();

void swap_write_page(u32 sec_no, void *page_data);

void swap_read_page(u32 sec_no, void *page_data);

#endif /* MM_SWAP_H_ */
