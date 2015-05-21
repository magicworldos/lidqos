/*
 * elf.h
 * Copyright (C) Feb 10, 2014 by lidq
 *
 * elf可重定位头文件
 */

#ifndef _ELF_H_
#define _ELF_H_

#include <kernel/typedef.h>

//ELF头前16字节固定长
#define EI_NIDENT 16

//ELF文件头
typedef struct elf32_hdr
{
	//固定的开始内容
	u8 e_ident[EI_NIDENT];
	//文件类型
	u16 e_type;
	//体系结构类型
	u16 e_machine;
	//当前版本号
	u32 e_version;
	//程序入口地址
	u32 e_entry;
	//程序头偏移地址
	u32 e_phoff;
	//节区头偏移地址
	u32 e_shoff;
	//标志
	u32 e_flags;
	//elf头大小
	u16 e_ehsize;
	//程序头大小
	u16 e_phentsize;
	//程序段数量
	u16 e_phnum;
	//节区头大小
	u16 e_shentsize;
	//节区头数量
	u16 e_shnum;
	//节区索引
	u16 e_shstrndx;
} Elf32_Ehdr;

//程序头
typedef struct elf32_phdr
{
	//类型
	u32 p_type;
	//程序偏移地址
	u32 p_offset;
	//段加载后在进程空间中占用的内存起始地址
	u32 p_vaddr;
	//在支持paging的OS中该字段被忽略
	u32 p_paddr;
	//该段在文件中占用的字节大小
	u32 p_filesz;
	//该段在内存中占用的字节大小
	u32 p_memsz;
	//标志
	u32 p_flags;
	//对齐
	u32 p_align;
} Elf32_Phdr;

//节区头
typedef struct elf32_shdr
{
	//节区名（实际是在shstrtab中的索引号）
	u32 sh_name;
	//节区类型
	u32 sh_type;
	//标志
	u32 sh_flags;
	//相对可执行地址
	u32 sh_addr;
	//偏移地址
	u32 sh_offset;
	//节区大小
	u32 sh_size;
	//其它节区索引
	u32 sh_link;
	//扩展信息
	u32 sh_info;
	//节区对齐
	u32 sh_addralign;
	//入口表大小
	u32 sh_entsize;
} Elf32_Shdr;

//符号表
typedef struct elf32_sym
{
	//在字符串中的索引号
	u32 st_name;
	//值，用于重定位
	u32 st_value;
	//大小
	u32 st_size;
	//符号信息
	u8 st_info;
	//其它信息
	u8 st_other;
	//节区索引
	u16 st_shndx;
} Elf32_Sym;

//重定位
typedef struct elf32_rel
{
	//需要重定位的偏移地址
	u32 r_offset;
	//其值为：(符号 << 8) | 重定位类型
	u32 r_info;
} Elf32_Rel;

#endif
