/*
 * pwd.c
 *
 *  Created on: Nov 24, 2014
 *      Author: lidq
 */

#ifndef _CAT_C_
#define _CAT_C_

#include <shell/stdio.h>
#include <shell/fs.h>
#include <shell/path.h>
#include <shell/stdlib.h>
#include <kernel/string.h>

s_session *session = NULL;

int main(int argc, char **args)
{
	if (argc >= 2)
	{
		session = malloc(sizeof(s_session));
		session->current_path = malloc(SHELL_CMD_LEN);

		int params[2];
		params[0] = 7;
		params[1] = (int) session;
		__asm__ volatile("int $0x80" :: "a"(params));

		//当前工作目录
		char *current_path = malloc(SHELL_CMD_LEN);
		params[0] = 6;
		params[1] = (int) current_path;
		__asm__ volatile("int $0x80" :: "a"(params));

		char *path = malloc(SHELL_CMD_LEN);
		str_copy(args[1], path);

		char *full_path = malloc(SHELL_CMD_LEN);
		full_path[0] = '\0';

		if (path[0] == '/')
		{
			repath(path, NULL, full_path);
		}
		//不是以'/'开头: 相对路径
		else
		{
			repath(path, current_path, full_path);
		}

		FILE *fp = fopen(full_path, FS_MODE_READ);
		if (fp == NULL)
		{
			printf("-bash: \"%s\": no such file or permission denied.\n", path);
		}
		else if (fp->fs.size > 0)
		{
			char *content = malloc(fp->fs.size);
			fread(fp, fp->fs.size, content);
			printf("%s\n", content);
			free(content);
		}
		fclose(fp);

		free(full_path);
		free(current_path);

		free(session->current_path);
		free(session);
	}
	return 0;
}

#endif
