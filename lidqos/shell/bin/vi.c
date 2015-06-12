#include <shell/vi.h>
#include <shell/stdio.h>
#include <shell/fs.h>
#include <shell/path.h>
#include <shell/stdlib.h>
#include <shell/curses.h>
#include <kernel/string.h>

s_session *session = NULL;

int main(int argc, char **args)
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

	//new file
	if (argc == 1)
	{

	}
	//open file
	else
	{

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

		int size = 0;
		void *data = NULL;
		FILE *fp = fopen(full_path, FS_MODE_READ);
		if (fp == NULL)
		{
			size = TTY_MEM_CH_SIZE;
			data = malloc(size);
		}
		else
		{
			size = fp->fs.size;
			data = malloc(size);
			fread(fp, size, data);
		}
		fclose(fp);

		edit_file(full_path, data, size);

		free(data);
	}

	return 0;
}

void edit_file(char *path, void *data, int size)
{
	initscr();

	addstr(data);

	getchar();

	endwin();
}
