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

void sem_wait_g(s_sem *sem);

void sem_post_g(s_sem *sem);

s_sem* get_global_sem(int type);

void sem_wait_shell(u32 sem_addr);

void sem_post_shell(u32 sem_addr);

#endif /* INCLUDE_KERNEL_SEMAPHORE_H_ */
