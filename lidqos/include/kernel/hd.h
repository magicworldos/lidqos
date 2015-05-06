/*
 * hd.h
 *
 *  Created on: May 6, 2015
 *      Author: lidq
 */

#ifndef _INCLUDE_KERNEL_HD_H_
#define _INCLUDE_KERNEL_HD_H_

#include <kernel/typedef.h>

u32 swap_alloc_page();

void swap_write_page(u32 sec_no, void *page_data);

void swap_read_page(u32 sec_no, void *page_data);

#endif /* INCLUDE_KERNEL_HD_H_ */
