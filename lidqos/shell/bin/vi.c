#include <shell/vi.h>

s_session *session = NULL;

s_vi_data *vi_data = NULL;

char *empty_line = NULL;

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

	vi_data = malloc(sizeof(s_vi_data));
	//最后一行为status栏
	vi_data->max_row = 24;
	vi_data->max_col = 80;
	vi_data->start_row = 0;
	vi_data->start_col = 0;
	vi_data->x = 0;
	vi_data->y = 0;
	//
	vi_data->status = 0;

	empty_line = malloc(vi_data->max_col + 1);
	for (int i = 0; i < vi_data->max_col; i++)
	{
		empty_line[i] = ' ';

	}
	empty_line[vi_data->max_col] = '\0';

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
		char *data = NULL;
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

	free(empty_line);

	free(vi_data);

	return 0;
}

void edit_file(char *path, char *data, int size)
{
	s_vdata *pvdata = convert_vdata(data);
	s_vdata *p = pvdata;
	for (int i = 0; i < vi_data->start_row && p != NULL; i++)
	{
		p = p->next;
	}

	initscr();

	char *buff = malloc(vi_data->max_col + 1);
	for (int i = 0; i < vi_data->max_row && p != NULL; i++)
	{
		memcpy(p->line, buff, vi_data->max_col);
		buff[vi_data->max_col] = '\0';
		addstr(buff);
		if (str_len(buff) < vi_data->max_col)
		{
			addch('\n');
		}
		p = p->next;
	}
	move(vi_data->x, vi_data->y);
	free(buff);

	while (1)
	{
		//0-2bit表示模式，0普通模式；1插入模式；2命令模式
		u8 mode = vi_data->status & 0x7;
		//normal mode
		if (mode == MODE_NORMAL)
		{
			mode_normal();
		}
		//insert mode
		else if (mode == MODE_INSERT)
		{
			mode_insert();
		}
		//cmd mode
		else if (mode == MODE_CMD)
		{
			if (mode_cmd() == -1)
			{
				break;
			}
		}
	}

	getchar();

	endwin();

}

void mode_normal()
{
	show_status("mode: normal");
	int key = 0;
	do
	{
		key = getkey();
		if (key == KEY_LEFT)
		{
			getxy(&vi_data->x, &vi_data->y);
			vi_data->x--;
			move(vi_data->x, vi_data->y);
		}
		else if (key == KEY_RIGHT)
		{
			getxy(&vi_data->x, &vi_data->y);
			vi_data->x++;
			move(vi_data->x, vi_data->y);
		}
		else if (key == KEY_UP)
		{
			getxy(&vi_data->x, &vi_data->y);
			vi_data->y--;
			move(vi_data->x, vi_data->y);
		}
		else if (key == KEY_DOWN)
		{
			getxy(&vi_data->x, &vi_data->y);
			vi_data->y++;
			move(vi_data->x, vi_data->y);
		}
		//insert
		else if (key == KEY_I)
		{
			switch_mode(MODE_INSERT);
			break;
		}
	}
	while (1);
}

void mode_insert()
{
	show_status("mode: insert");
	char key = 0;
	do
	{
		key = getkey();
	}
	while (1);
}

int mode_cmd()
{
	show_status("mode: cmd");
	char key = 0;
	do
	{
		key = getkey();
	}
	while (1);
	return 0;
}

void show_status(char *str)
{
	move(0, 24);
	addstr(empty_line);
	move(0, 24);
	addstr(str);
	move(vi_data->x, vi_data->y);
}

void switch_mode(u8 mode)
{
	vi_data->status &= 0xfffffff8;
	vi_data->status |= mode;
}

int line_length(char *data, int start)
{
	int pos = 0;
	for (pos = 0; data[start + pos] != '\0'; pos++)
	{
		if (data[start + pos] == '\n')
		{
			break;
		}
	}
	return ++pos;
}

s_vdata* convert_vdata(char *data)
{
	s_vdata* header = NULL;
	s_vdata* pre = NULL;
	int total_length = str_len(data);
	if (total_length == 0)
	{
		return NULL;
	}

	int start = 0;
	do
	{
		int length = line_length(data, start);
		s_vdata* pvdata = malloc(sizeof(s_vdata));
		pvdata->line = malloc(length);
		memcpy(&data[start], pvdata->line, length);
		pvdata->line[length - 1] = '\0';
		pvdata->next = NULL;
		if (header == NULL)
		{
			header = pvdata;
			pre = pvdata;
		}
		else
		{
			pre->next = pvdata;
			pre = pvdata;
		}

		start += length;
	}
	while (start < total_length);

	return header;
}
