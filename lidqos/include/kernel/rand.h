/*
 * rand.h
 *
 *  Created on: May 26, 2015
 *      Author: lidq
 */

#ifndef _INCLUDE_KERNEL_RAND_H_
#define _INCLUDE_KERNEL_RAND_H_

#include <kernel/typedef.h>

int rand();

void srand(u32 seed);

int random(int min, int max);

#endif /* INCLUDE_KERNEL_RAND_H_ */
