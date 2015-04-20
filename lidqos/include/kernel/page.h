/*
 * page.h
 *
 *  Created on: Apr 20, 2015
 *      Author: lidq
 */

#ifndef _INCLUDE_KERNEL_PAGE_H_
#define _INCLUDE_KERNEL_PAGE_H_

#include <kernel/typedef.h>

#define PAGE_SIZE			(4 * 1024)
#define MEMORY_RANGE 		(4 * 1024 * 1024)
#define PAGE_DIR			(0x300000)
#define PAGE_TABLE			(PAGE_DIR + PAGE_SIZE)

void install_page();

#endif /* INCLUDE_KERNEL_PAGE_H_ */
