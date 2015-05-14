/*
 * hd.h
 *
 *  Created on: May 6, 2015
 *      Author: lidq
 */

#ifndef _INCLUDE_KERNEL_HD_H_
#define _INCLUDE_KERNEL_HD_H_

#include <kernel/typedef.h>
#include <kernel/io.h>

#define HD_PORT_DATA            0x1f0
#define HD_PORT_ERROR           0x1f1
#define HD_PORT_SECT_COUNT      0x1f2
#define HD_PORT_LBA0	        0x1f3
#define HD_PORT_LBA1			0x1f4
#define HD_PORT_LBA2	        0x1f5
#define HD_PORT_LBA3	        0x1f6
#define HD_PORT_STATUS          0x1f7
#define HD_PORT_COMMAND         0x1f7

#define HD_READ         		0x20
#define HD_WRITE        		0x30

void hd_rw(u32 lba, u8 com, void *buf);

void install_swap();

u32 swap_alloc_sec();

void swap_free_sec(u32 sec_no);

u32 swap_alloc_page();

void swap_write_page(u32 sec_no, void *page_data);

void swap_read_page(u32 sec_no, void *page_data);

#endif /* INCLUDE_KERNEL_HD_H_ */
