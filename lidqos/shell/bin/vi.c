#include <shell/vi.h>

s_session *session = NULL;

s_vi_data *vi_data = NULL;

s_vdata *header = NULL;

char *empty_line = NULL;

char *buff = NULL;

char *full_path = NULL;

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
	vi_data->last_col_no = vi_data->x;
	//
	vi_data->status = 0;
	vi_data->line_count = 0;

	empty_line = malloc(vi_data->max_col + 1);
	for (int i = 0; i < vi_data->max_col; i++)
	{
		empty_line[i] = ' ';

	}
	empty_line[vi_data->max_col] = '\0';

	full_path = malloc(SHELL_CMD_LEN);
	full_path[0] = '\0';

	//new file
	if (argc == 1)
	{

	}
	//open file
	else
	{
		char *path = malloc(SHELL_CMD_LEN);
		str_copy(args[1], path);

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

		edit_file(data, size);

		free(data);
	}

	free(full_path);

	free(empty_line);

	free(vi_data);

	return 0;
}

void edit_file(char *data, int size)
{
	header = convert_vdata(data);

	initscr();

	buff = malloc(vi_data->max_col + 1);

	all_line();

	move(vi_data->x, vi_data->y);

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

	endwin();

	free(buff);

}

void mode_normal()
{
	show_status("NORMAL");

	char ch = 0;
	int key = 0;
	do
	{
		key = getkey(&ch);
		//insert
		if (key == KEY_I)
		{
			switch_mode(MODE_INSERT);
			break;
		}
		else if (ch == ':')
		{
			switch_mode(MODE_CMD);
			break;
		}
		else if (key == KEY_LEFT)
		{
			scroll(key);
		}
		else if (key == KEY_RIGHT)
		{
			scroll(key);
		}
		else if (key == KEY_UP)
		{
			scroll(key);
		}
		else if (key == KEY_DOWN)
		{
			scroll(key);
		}
		else if (key == KEY_HOME)
		{
			scroll(key);
		}
		else if (key == KEY_END)
		{
			scroll(key);
		}
		else if (key == KEY_PAGE_UP)
		{
			scroll(key);
		}
		else if (key == KEY_PAGE_DOWN)
		{
			scroll(key);
		}

	}
	while (1);
}

void mode_insert()
{
	show_status("INSERT");

	int key = 0;
	char ch = 0;
	do
	{
		key = getkey(&ch);
		//ESC
		if (key == KEY_ESC)
		{
			switch_mode(MODE_NORMAL);
			break;
		}
		else if (key == KEY_LEFT)
		{
			scroll(key);
		}
		else if (key == KEY_RIGHT)
		{
			scroll(key);
		}
		else if (key == KEY_UP)
		{
			scroll(key);
		}
		else if (key == KEY_DOWN)
		{
			scroll(key);
		}
		else if (key == KEY_HOME)
		{
			scroll(key);
		}
		else if (key == KEY_END)
		{
			scroll(key);
		}
		else if (key == KEY_PAGE_UP)
		{
			scroll(key);
		}
		else if (key == KEY_PAGE_DOWN)
		{
			scroll(key);
		}
		else if (key == KEY_BACKSPACE)
		{
			backspace_char();
		}
		else if (key == KEY_DEL)
		{
			delete_char();
		}
		else if (key == KEY_ENTER)
		{
			newline_char();
		}
		else
		{
			int line_count = 0;
			s_vdata *p = current_line(&line_count);
			insert_char(p, ch);
		}
	}
	while (1);

}

int mode_cmd()
{
	show_status("COMMAND");

	int ret = 0;
	char *cmd = malloc(0x200);
	move(0, 24);
	addstr(empty_line);
	move(0, 24);
	addch(':');
	get_cmd(cmd);
	for (int i = 0; i < str_len(cmd); i++)
	{
		if (cmd[i] == 'w')
		{
			if ((ret = save()) == 1)
			{
				set_file_status(0);
			}
		}
		else if (cmd[i] == 'q')
		{
			if (file_status() == 0)
			{
				ret = -1;
			}
			else if (cmd[i + 1] == '!')
			{
				ret = -1;
			}
			else if (cmd[i + 1] != '!')
			{
				ret = 0;
				show_status_msg("file is changed, please save it first or use the \"q!\" cmd.");
			}
			break;
		}
	}
	free(cmd);
	move(vi_data->x, vi_data->y);
	switch_mode(MODE_NORMAL);

	return ret;
}

void get_cmd(char *str)
{
	char ch, *p = str;
	while ((ch = getchar()) != '\n')
	{
		if (ch == 0x8)
		{
			if (p > str)
			{
				int back_num = 1;
				if (*(p - 1) == '\t')
				{
					back_num = 4;
				}
				for (int i = 0; i < back_num; ++i)
				{
					p--;
					backspace();
				}
			}
		}
		else
		{
			*p = ch;
			p++;
			putchar(ch);
		}
	}
	*p = '\0';
}

int save()
{
	s_vdata *p = header;

	FILE *fp = fopen(full_path, FS_MODE_WRITE);
	if (fp == NULL)
	{
		show_status_msg("write file error.");
		return 0;
	}
	while (p != NULL)
	{
		p->line[p->length] = '\n';
		fwrite(fp, p->length, p->line);
		p->line[p->length] = '\0';
		fwrite(fp, 1, "\n");
		p = p->next;
	}
	fclose(fp);
	show_status_msg("file is written.");
	return 1;
}

void show_status(char *str)
{
	move(70, 24);
	addstr(empty_line);
	move(70, 24);
	addstr(str);
	move(vi_data->x, vi_data->y);
}

void show_status_msg(char *str)
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

int file_status()
{
	return (vi_data->status >> 3) & 0x1;
}

void set_file_status(int changed)
{
	if (changed)
	{
		vi_data->status |= 0x8;
	}
	else
	{
		vi_data->status &= 0xfffffff7;
	}
}

void scroll(int key)
{
	if (key == KEY_LEFT)
	{
		getxy(&vi_data->x, &vi_data->y);
		vi_data->x--;
		if (vi_data->x < 0)
		{
			if (vi_data->start_col > 0)
			{
				vi_data->start_col -= (vi_data->max_col / 2);
				vi_data->x = (vi_data->max_col / 2) - 1;
			}
			else
			{
				vi_data->x = 0;
			}
			all_line();
		}
		move(vi_data->x, vi_data->y);
		vi_data->last_col_no = vi_data->x;
	}
	else if (key == KEY_RIGHT)
	{
		getxy(&vi_data->x, &vi_data->y);
		vi_data->x++;
		s_vdata *p = current_line();
		if (vi_data->x >= vi_data->max_col)
		{
			vi_data->start_col += (vi_data->max_col / 2);
			vi_data->x = (vi_data->max_col / 2);
			all_line();
		}
		if (vi_data->x + vi_data->start_col >= p->length)
		{
			if (p->length - vi_data->start_col >= 0)
			{
				vi_data->x = p->length - vi_data->start_col;
			}
			else
			{
				vi_data->x = p->length % (vi_data->max_col / 2);
				vi_data->start_col = (p->length / (vi_data->max_col / 2)) * (vi_data->max_col / 2);
			}
		}
		move(vi_data->x, vi_data->y);
		vi_data->last_col_no = vi_data->x;
	}
	else if (key == KEY_UP)
	{
		getxy(&vi_data->x, &vi_data->y);
		vi_data->x = vi_data->last_col_no;
		vi_data->y--;
		if (vi_data->y < 0)
		{
			if (vi_data->start_row > 0)
			{
				vi_data->start_row--;
			}
			vi_data->y = 0;
		}
		s_vdata *p = current_line();
		if (vi_data->x + vi_data->start_col >= p->length)
		{
			if (p->length - vi_data->start_col >= 0)
			{
				vi_data->x = p->length - vi_data->start_col;
			}
			else
			{
				vi_data->x = p->length % (vi_data->max_col / 2);
				vi_data->start_col = (p->length / (vi_data->max_col / 2)) * (vi_data->max_col / 2);
			}
		}
		all_line();
		move(vi_data->x, vi_data->y);
	}
	else if (key == KEY_DOWN)
	{
		getxy(&vi_data->x, &vi_data->y);
		vi_data->x = vi_data->last_col_no;
		vi_data->y++;
		if (vi_data->y + vi_data->start_row >= vi_data->line_count)
		{
			vi_data->y--;
			return;
		}

		if (vi_data->y >= vi_data->max_row)
		{
			vi_data->y = vi_data->max_row - 1;
			vi_data->start_row++;
			all_line();
		}
		s_vdata *p = current_line();
		if (vi_data->x + vi_data->start_col >= p->length)
		{
			if (p->length - vi_data->start_col >= 0)
			{
				vi_data->x = p->length - vi_data->start_col;
			}
			else
			{
				vi_data->x = p->length % (vi_data->max_col / 2);
				vi_data->start_col = (p->length / (vi_data->max_col / 2)) * (vi_data->max_col / 2);
			}
			all_line();
		}
		move(vi_data->x, vi_data->y);
	}
	else if (key == KEY_HOME)
	{
		getxy(&vi_data->x, &vi_data->y);
		vi_data->x = 0;
		vi_data->start_col = 0;
		all_line();
		move(vi_data->x, vi_data->y);
		vi_data->last_col_no = vi_data->x;
	}
	else if (key == KEY_END)
	{
		getxy(&vi_data->x, &vi_data->y);
		s_vdata *p = current_line();
		vi_data->x = p->length % (vi_data->max_col / 2);
		vi_data->start_col = (p->length / (vi_data->max_col / 2)) * (vi_data->max_col / 2);
		all_line();
		move(vi_data->x, vi_data->y);
		vi_data->last_col_no = vi_data->x;
	}
	else if (key == KEY_PAGE_UP)
	{
		getxy(&vi_data->x, &vi_data->y);
		vi_data->x = vi_data->last_col_no;

		vi_data->start_row -= vi_data->max_row;
		if (vi_data->start_row < 0)
		{
			vi_data->start_row = 0;
		}
		if (vi_data->y + vi_data->start_row < vi_data->max_row)
		{
			vi_data->y = 0;
		}
		s_vdata *p = current_line();
		if (vi_data->x + vi_data->start_col >= p->length)
		{
			if (p->length - vi_data->start_col >= 0)
			{
				vi_data->x = p->length - vi_data->start_col;
			}
			else
			{
				vi_data->x = p->length % (vi_data->max_col / 2);
				vi_data->start_col = (p->length / (vi_data->max_col / 2)) * (vi_data->max_col / 2);
			}
		}
		all_line();
		move(vi_data->x, vi_data->y);
	}
	else if (key == KEY_PAGE_DOWN)
	{
		getxy(&vi_data->x, &vi_data->y);
		vi_data->x = vi_data->last_col_no;

		vi_data->start_row += vi_data->max_row;
		if (vi_data->y + vi_data->start_row >= vi_data->line_count)
		{
			vi_data->y = vi_data->line_count % vi_data->max_row - 1;
			vi_data->start_row = vi_data->line_count / vi_data->max_row * vi_data->max_row;
		}
		s_vdata *p = current_line();
		if (vi_data->x + vi_data->start_col >= p->length)
		{
			if (p->length - vi_data->start_col >= 0)
			{
				vi_data->x = p->length - vi_data->start_col;
			}
			else
			{
				vi_data->x = p->length % (vi_data->max_col / 2);
				vi_data->start_col = (p->length / (vi_data->max_col / 2)) * (vi_data->max_col / 2);
			}
		}
		all_line();
		move(vi_data->x, vi_data->y);

	}
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
		pvdata->mm_length = length;
		pvdata->length = str_len(pvdata->line);
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

		vi_data->line_count++;
	}
	while (start < total_length);

	if (header == NULL)
	{
		s_vdata* pvdata = malloc(sizeof(s_vdata));
		pvdata->line = malloc(4);
		memcpy("", pvdata->line, 4);
		pvdata->line[0] = '\0';
		pvdata->mm_length = 8;
		pvdata->length = 0;
		pvdata->next = NULL;

		header = pvdata;
	}

	return header;
}

void insert_char(s_vdata *p, char ch)
{
	set_file_status(1);

	char *from = &p->line[vi_data->x];
	if (p->length >= p->mm_length)
	{
		p->line = realloc(p->line, p->mm_length * 2);
		p->mm_length *= 2;
	}

	for (int i = p->length; i > vi_data->x + vi_data->start_col; i--)
	{
		p->line[i] = p->line[i - 1];
	}
	p->line[vi_data->x + vi_data->start_col] = ch;
	p->line[p->length + 1] = '\0';
	p->length++;

	move(0, vi_data->y);
	if (p->length <= vi_data->start_col)
	{
		addstr_with_buff("");
	}
	else
	{
		addstr_with_buff(p->line + vi_data->start_col);
	}
	move(vi_data->x, vi_data->y);
	scroll(KEY_RIGHT);
}

void backspace_char()
{
	set_file_status(1);

	if (vi_data->x == 0 && vi_data->y == 0 && vi_data->start_col == 0 && vi_data->start_row == 0)
	{
		return;
	}

	s_vdata *p = current_line();

	if (vi_data->x == 0 && vi_data->start_col == 0)
	{
		s_vdata *pre = header;
		for (int i = 0; i < vi_data->y - 1 && pre != NULL; i++)
		{
			pre = pre->next;
		}
		int pre_length = pre->length;
		pre->mm_length += p->mm_length;
		pre->line = realloc(pre->line, pre->mm_length);

		int j = pre->length;
		for (int i = 0; i < p->length; i++, j++)
		{
			pre->line[j] = p->line[i];
		}
		pre->line[j] = '\0';
		pre->length = j;
		pre->next = p->next;

		free(p->line);
		free(p);

		vi_data->line_count--;

		all_line();

		move(vi_data->x, vi_data->y);
		scroll(KEY_UP);
		scroll(KEY_HOME);
		for (int i = 0; i < pre_length; i++)
		{
			scroll(KEY_RIGHT);
		}
		return;
	}

	for (int i = vi_data->x + vi_data->start_col; i < p->length; i++)
	{
		p->line[i - 1] = p->line[i];
	}
	p->line[p->length - 1] = '\0';
	p->length--;

	move(0, vi_data->y);
	if (p->length <= vi_data->start_col)
	{
		addstr_with_buff("");
	}
	else
	{
		addstr_with_buff(p->line + vi_data->start_col);
	}
	move(vi_data->x, vi_data->y);
	scroll(KEY_LEFT);
}

void delete_char()
{
	set_file_status(1);

	s_vdata *p = current_line();
	if (vi_data->x + vi_data->start_col >= p->length && p->next == NULL)
	{
		return;
	}

	if (vi_data->x + vi_data->start_col >= p->length)
	{
		s_vdata *pn = p->next;
		p->mm_length += pn->mm_length;
		p->line = realloc(p->line, p->mm_length);

		int j = p->length;
		for (int i = 0; i < pn->length; i++, j++)
		{
			p->line[j] = pn->line[i];
		}
		p->line[j] = '\0';
		p->length = j;
		p->next = pn->next;

		free(pn->line);
		free(pn);

		vi_data->line_count--;

		all_line();

		move(vi_data->x, vi_data->y);
		return;
	}

	for (int i = vi_data->x + vi_data->start_col; i < p->length; i++)
	{
		p->line[i] = p->line[i + 1];
	}
	p->line[p->length - 1] = '\0';
	p->length--;

	move(0, vi_data->y);
	if (p->length <= vi_data->start_col)
	{
		addstr_with_buff("");
	}
	else
	{
		addstr_with_buff(p->line + vi_data->start_col);
	}
	move(vi_data->x, vi_data->y);
}

void newline_char()
{
	set_file_status(1);

	s_vdata *p = current_line();
	s_vdata *newline = malloc(sizeof(s_vdata));

	if (vi_data->x + vi_data->start_col >= p->length)
	{
		newline->line = malloc(4);
		newline->length = 0;
		newline->mm_length = 4;
		newline->line[0] = '\0';
	}
	else
	{
		newline->line = malloc(p->mm_length);
		newline->mm_length = p->mm_length;

		int j = 0;
		for (int i = vi_data->x + vi_data->start_col; p->line[i] != '\0'; i++, j++)
		{
			newline->line[j] = p->line[i];
		}
		newline->line[j] = '\0';
		newline->length = j;

		p->line[vi_data->x + vi_data->start_col] = '\0';
		p->length = vi_data->x + vi_data->start_col;
	}

	newline->next = p->next;
	p->next = newline;

	vi_data->line_count++;

	all_line();

	move(vi_data->x, vi_data->y);

	scroll(KEY_DOWN);
	scroll(KEY_HOME);
}

void empty_buff(char *buff, int length)
{
	for (int i = 0; i < length; i++)
	{
		buff[i] = ' ';
	}
}

s_vdata* current_line()
{
	s_vdata *p = header;
	for (int i = 0; i < vi_data->y + vi_data->start_row && p != NULL; i++)
	{
		p = p->next;
	}
	return p;
}

void addstr_with_buff(char *line)
{
	empty_buff(buff, vi_data->max_col);
	buff[vi_data->max_col] = '\0';
	move(0, vi_data->y);
	addstr(buff);
	memcpy(line, buff, vi_data->max_col);
	buff[vi_data->max_col] = '\0';
	move(0, vi_data->y);
	addstr(buff);
}

void all_line()
{
	s_vdata *p = header;
	for (int i = 0; i < vi_data->start_row && p != NULL; i++)
	{
		p = p->next;
	}

	for (int i = 0; i < vi_data->max_row; i++)
	{
		empty_buff(buff, vi_data->max_col);
		move(0, i);
		addstr(buff);
		if (p != NULL)
		{
			if (p->length <= vi_data->start_col)
			{
				memcpy("", buff, vi_data->max_col);
			}
			else
			{
				memcpy(p->line + vi_data->start_col, buff, vi_data->max_col);
			}
			buff[vi_data->max_col] = '\0';
			move(0, i);
			addstr(buff);
			p = p->next;
		}
	}
}
