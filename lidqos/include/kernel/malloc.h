/*
 * alloc.h
 * Copyright (C) Feb 10, 2014 by lidq
 *
 * 内存申请头文件
 */

#ifndef _MALLOC_H_
#define _MALLOC_H_

#ifndef MK_ISO_FS

#include <kernel/typedef.h>
#include <kernel/sys_var.h>
#include <kernel/page.h>
#include <kernel/alloc.h>

void* palloc(s_pcb *pcb, int page);

void pfree(s_pcb *pcb, void *addr, int page);

void* malloc(s_pcb *pcb, int size);

void free(s_pcb *pcb, void *addr, int size);

#endif

#endif
