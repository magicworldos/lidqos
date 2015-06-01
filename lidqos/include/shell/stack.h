/*
 * statck.h
 *
 *  Created on: Feb 26, 2015
 *      Author: lidq
 */

#ifndef _INCLUDE_SHELL_STATCK_H_
#define _INCLUDE_SHELL_STATCK_H_

#include <kernel/typedef.h>
#include <shell/stdlib.h>

typedef struct
{
	u8 *top;
	u8 *bottom;

	int unit_size;
	int unit_count;

	u8 *addr;
	int size;
} s_stack;

s_stack* stack_init(int unit_size, int unit_count);

void stack_free(s_stack* sp);

int stack_push(s_stack* sp, void *unit);

int stack_pop(s_stack* sp, void *unit);

int stack_count(s_stack* sp);

#endif /* INCLUDE_SHELL_STATCK_H_ */
