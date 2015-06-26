/*
 * pwd.c
 *
 *  Created on: Nov 24, 2014
 *      Author: lidq
 */

#include <kernel/io.h>
#include <shell/stdlib.h>
#include <shell/stdio.h>

int main(int argc, char **args)
{
	//当前工作目录
	char *current_path = malloc(SHELL_CMD_LEN);
	int params[2];
	params[0] = 6;
	params[1] = (int) current_path;
	__asm__ volatile("int $0x80" :: "a"(params));
	printf("%s\n", current_path);
	free(current_path);

// 测试用，普通程序不能进程IO端口操作
//	u32 pos = 10;
//	outb_p(14, 0x03d4);
//	outb_p((pos >> 8) & 0xff, 0x03d5);
//	outb_p(15, 0x03d4);
//	outb_p(pos & 0xff, 0x03d5);

	return 0;
}
