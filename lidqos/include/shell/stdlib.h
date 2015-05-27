/*
 * stdio.h
 *
 *  Created on: May 26, 2015
 *      Author: lidq
 */

#ifndef _INCLUDE_SHELL_STDLIB_H_
#define _INCLUDE_SHELL_STDLIB_H_

#include <kernel/typedef.h>

int rand();

void srand(u32 seed);

int random(int min, int max);

#endif /* INCLUDE_SHELL_STDIO_H_ */