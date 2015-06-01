/*
 * stack.c
 *
 *  Created on: Feb 26, 2015
 *      Author: lidq
 */

#ifndef _STACK_C_
#define _STACK_C_

#include <shell/stack.h>

s_stack* stack_init(int unit_size, int unit_count)
{
	s_stack *sp = malloc(sizeof(s_stack));

	sp->unit_size = unit_size;
	sp->unit_count = 0;

	sp->size = unit_size * unit_count;
	sp->addr = malloc(sp->size);
	sp->top = sp->addr + sp->size;
	sp->bottom = sp->top;

	return sp;
}

void stack_free(s_stack* sp)
{
	free(sp->addr);
	free(sp);
}

int stack_push(s_stack* sp, void *unit)
{
	if (sp->top < sp->addr)
	{
		return 0;
	}
	sp->top -= (u32)sp->unit_size;
	memcpy(unit, sp->top, sp->unit_size);
	sp->unit_count++;
	return 1;
}

int stack_pop(s_stack* sp, void *unit)
{
	if (sp->unit_count <= 0)
	{
		return 0;
	}
	memcpy(sp->top, unit, sp->unit_size);
	sp->top += sp->unit_size;
	sp->unit_count--;

	return 1;

}

int stack_count(s_stack* sp)
{
	return sp->unit_count;
}

#endif
