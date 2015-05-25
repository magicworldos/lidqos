/*
 * semaphore.h
 *
 *  Created on: May 25, 2015
 *      Author: lidq
 */

#ifndef _INCLUDE_KERNEL_SEMAPHORE_H_
#define _INCLUDE_KERNEL_SEMAPHORE_H_

#include <kernel/typedef.h>

void sem_init(s_sem *sem, int value);

void sem_wait(s_sem *sem);

void sem_post(s_sem *sem);

#endif /* INCLUDE_KERNEL_SEMAPHORE_H_ */
