/*
 * example_file.c
 *
 *  Created on: Jun 9, 2015
 *      Author: lidq
 */

#include <shell/stdio.h>
#include <shell/stdlib.h>
#include <shell/fs.h>

s_session *session = NULL;

int main(int argc, char **args)
{
	session = malloc(sizeof(s_session));
	session->current_path = malloc(SHELL_CMD_LEN);

	int params[2];
	params[0] = 7;
	params[1] = (int) session;
	__asm__ volatile("int $0x80" :: "a"(params));

	FILE *fp = fopen(args[1], FS_MODE_WRITE);
	if (fp == NULL)
	{
		printf("Write file: %s error.\n", args[1]);
	}
	else
	{
		char *str = malloc(0x200);
		scanf("%s", str);
		fwrite(fp, str_len(str), str);
		free(str);
	}
	fclose(fp);

	free(session->current_path);
	free(session);

	return 0;
}
