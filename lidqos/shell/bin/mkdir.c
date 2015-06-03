/*
 * mkdir.c
 *
 *  Created on: Nov 25, 2014
 *      Author: lidq
 */

#ifndef _MKDIR_C_
#define _MKDIR_C_

#include <shell/stdio.h>
#include <shell/fs.h>
#include <shell/path.h>
#include <shell/stdlib.h>
#include <kernel/string.h>

s_session *session = NULL;

int is_exist(char *folder_name)
{
	int ret = 0;
	char *path = malloc(SHELL_CMD_LEN);
	str_copy(folder_name, path);
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
	if (status == 1)
	{
		ret = 0;
	}
	else
	{
		ret = 1;
	}

	free(fs_work);
	free(full_path);
	free(path);

	return ret;
}

int main(int argc, char **args)
{
	for (int i = 1; i < argc; ++i)
	{
		session = malloc(sizeof(s_session));
		session->current_path = malloc(SHELL_CMD_LEN);

		int params[2];
		params[0] = 7;
		params[1] = (int) session;
		__asm__ volatile("int $0x80" :: "a"(params));

		if (is_exist(args[i]))
		{
			printf("-bash : \"%s\": the folder already exists.\n", args[i]);
		}
		else
		{
			char f_name[0x200];
			str_copy(args[i], f_name);
			int len = str_len(f_name);
			if (f_name[len - 1] != '/')
			{
				f_name[len] = '/';
				f_name[len + 1] = '\0';
			}
			int s = fs_create_folder(session->current_path, f_name, session->uid, session->uid, 01755);
			if (s == -1)
			{
				printf("-bash : \"%s\": permission denied.\n", args[i]);
			}
			else
			{
				printf("-bash : create folder \"%s\" sucessfully.\n", args[i]);
			}
		}
		free(session->current_path);
		free(session);
	}
	return 0;
}

#endif
