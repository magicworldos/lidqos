/*
 * ls.c
 *
 *  Created on: Nov 25, 2014
 *      Author: lidq
 */
#ifndef _RM_C_
#define _RM_C_

#include <shell/stdio.h>
#include <shell/fs.h>
#include <shell/path.h>
#include <shell/stdlib.h>
#include <kernel/string.h>

s_session *session = NULL;

void del_file(char *path);
void del_dir(char *path);

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

		u8 flag = 0x0;
		char *path = malloc(0x200);
		for (int i = 1; i < argc; i++)
		{
			if (args[i][0] == '-')
			{
				for (int j = 1; j < str_len(args[i]); j++)
				{
					if (args[i][j] == 'r')
					{
						//递归删除，通指删除文件夹
						flag |= (0x1 << 0);
					}
					else if (args[i][j] == 'f')
					{
						//强制删除，通指只读文件或文件夹
						flag |= (0x1 << 1);
					}
				}
			}
			else
			{
				str_copy(args[i], path);
			}
		}

		if (str_len(path) > 0)
		{
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
			FILE *fp = fopen(full_path, FS_MODE_READ);
			if (fp != NULL)
			{
				fclose(fp);
				del_file(full_path);
				return 0;
			}
			fclose(fp);
			int len = str_len(full_path);
			if (full_path[len - 1] != '/')
			{
				full_path[len] = '/';
				full_path[len + 1] = '\0';
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
				printf("-bash: \"%s\": permission denied.\n", path);
			}
			else
			{
				//-r
				if ((flag >> 0) & 0x1 == 1)
				{
					del_dir(full_path);
				}
				else
				{
					printf("rm: cannot remove \"%s\": is a directory.\n", path);
				}
			}
			free(fs_work);
			free(full_path);
		}

		free(path);

		free(session->current_path);
		free(session);
	}

	return 0;
}

void del_file(char *path)
{
	s_fs *fs_work = malloc(sizeof(s_fs));
	u32 dev_id = 0;
	int status = fs_find_path_by_user(path, session->uid, session->gid, &dev_id, fs_work);

	if (status == FS_STATUS_NO_FILE_DIR)
	{
		printf("-bash: \"%s\": no such file or directory.\n", path);
	}
	else if (status == FS_STATUS_NO_PERMISSION)
	{
		printf("-bash: \"%s\": permission denied.\n", path);
	}
	free(fs_work);

	status = 0;
	int params[5];
	params[0] = 4;
	params[1] = (int) path;
	params[2] = (int) session->uid;
	params[3] = (int) session->gid;
	params[4] = (int) &status;
	__asm__ volatile("int	$0x87" :: "a"(params));

	if (status == 0)
	{
		printf("rm: removed \"%s\".\n", path);
	}
	else if (status == -1)
	{
		printf("-bash: \"%s\": permission denied.\n", path);
	}
}

void del_dir(char *path)
{
	FILE *fp = malloc(sizeof(FILE));
	FILE *p = fp;
	fopendir(path, fp);

	while (p->fs.dot != 0)
	{
		char *file_path = malloc(0x200);
		repath(p->fs.name, path, file_path);
		if ((p->fs.mode >> 9) & 0x1 == 0)
		{
			del_file(file_path);
		}
		else
		{
			int len = str_len(file_path);
			if (file_path[len - 1] != '/')
			{
				file_path[len] = '/';
				file_path[len + 1] = '\0';
			}
			del_dir(file_path);
		}
		free(file_path);
		fdirnext(p);
	}
	fclosedir(fp);
	del_file(path);
	free(fp);
}

#endif

