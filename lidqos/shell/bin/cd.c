/*
 * cd.c
 *
 *  Created on: Nov 24, 2014
 *      Author: lidq
 */

#ifndef _CD_C_
#define _CD_C_

#include <kernel/string.h>
#include <shell/stdio.h>
#include <shell/stdlib.h>
#include <shell/fs.h>
#include <shell/path.h>

s_session *session = NULL;

int main(int argc, char **args)
{
	if (argc >= 2)
	{
		s_session *session = malloc(sizeof(s_session));
		session->current_path = malloc(SHELL_CMD_LEN);

		int params[2];
		params[0] = 7;
		params[1] = (int) session;
		__asm__ volatile("int $0x80" :: "a"(params));

		char *path = malloc(SHELL_CMD_LEN);
		str_copy(args[1], path);
		int len = str_len(path);
		if (path[len - 1] != '/')
		{
			path[len] = '/';
			path[len + 1] = '\0';
		}

		char *full_path = malloc(SHELL_CMD_LEN);
		full_path[0] = '\0';

		if (path[0] == '/')
		{
			repath(path, NULL, full_path);
		}
		//不是以'/'开头: 相对路径
		else
		{
			repath(path, session->current_path, full_path);
		}
		s_fs *fs_work = malloc(sizeof(s_fs));
		u32 dev_id = 0;
		int status = fs_find_path_by_user(full_path, session->uid, session->gid, &dev_id, fs_work);

		if (status == FS_STATUS_NO_FILE_DIR)
		{
			printf("-bash: \"%s\": no such file or directory.\n", path);
		}
		else if (status == FS_STATUS_NO_PERMISSION)
		{
			printf("-bash: \"%s\": Permission denied.\n", path);
		}
		else
		{
			str_copy(full_path, session->current_path);
		}

		params[0] = 8;
		params[1] = (int) session->current_path;
		__asm__ volatile("int $0x80" :: "a"(params));

		free(fs_work);
		free(full_path);

		free(session->current_path);
		free(session);
	}
	return 0;
}

#endif
