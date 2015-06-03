/*
 * ls.c
 *
 *  Created on: Nov 25, 2014
 *      Author: lidq
 */
#ifndef _LS_C_
#define _LS_C_

#include <shell/stdio.h>
#include <shell/fs.h>
#include <shell/stdlib.h>
#include <kernel/string.h>

int main(int argc, char **args)
{
	//当前工作目录
	char *current_path = malloc(SHELL_CMD_LEN);
	int params[2];
	params[0] = 6;
	params[1] = (int) current_path;
	__asm__ volatile("int $0x80" :: "a"(params));

	FILE *fp = malloc(sizeof(FILE));
	FILE *p = fp;
	fopendir(current_path, fp);
	while (p->fs.dot != 0)
	{
		printf("%d %d %o %s\n", p->fs.owner, p->fs.group, p->fs.mode, p->fs.name);
		fdirnext(p);
	}
	fclosedir(fp);

	free(fp);
	free(current_path);

	return 0;
}

#endif

