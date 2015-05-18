/*
 * sys_var.h
 *
 *  Created on: Dec 1, 2014
 *      Author: lidq
 */

#ifndef _SYS_VAR_H_
#define _SYS_VAR_H_

#include <kernel/typedef.h>

//浮点数据缓冲区大小
#define FPU_SIZE				(0x200)

//tss数据结构
typedef struct tss_s
{
	u32 back_link;
	u32 esp0, ss0;
	u32 esp1, ss1;
	u32 esp2, ss2;
	u32 cr3;
	u32 eip;
	u32 eflags;
	u32 eax, ecx, edx, ebx;
	u32 esp, ebp;
	u32 esi, edi;
	u32 es, cs, ss, ds, fs, gs;
	u32 ldt;
	u32 trace_bitmap;
} s_tss;

typedef struct alloc_list_s
{
	void* addr;
	int size;
	struct alloc_list_s *next;

} s_alloc_list;

typedef struct process_control_block
{
	//pcb id
	int process_id;
	s_tss tss;
	s_gdt ldt[2];
} s_pcb;

typedef struct
{
	//header
	s_pcb *ph;
	//current
	s_pcb *pc;
	//run
	s_pcb *pr;
	//last run
	s_pcb *plr;
} s_pcb_info;

typedef struct s_hda_rw
{
	u32 lba;
	u8 com;
	u8* buff;
	int* status;

	struct s_hda_rw *next;
} s_hda_rw;

typedef struct s_partition_table
{
	u8 device;
	u8 boot;
	u8 id;
	u8 type;
	u32 bitmap_start;
	u32 start;
	u32 size;
} s_pt;

typedef struct
{
	s_hda_rw *hda_rw_header;
	s_hda_rw *hda_rw_footer;

	int pt_count;
	s_pt *pts;
} s_hda_info;

typedef struct
{
	s_pcb_info *pcb_info;
	s_hda_info *hda_info;

} s_sys_var;

#endif /* SYS_VAR_H_ */
