/*
 * shell.c
 *
 *  Created on: Nov 26, 2014
 *      Author: lidq
 */

#include <shell/shell.h>

s_session *session = NULL;

int main(int argc, char **args)
{
	//设置计算机名（以后要从/etc/hostname里读入）
	char *computer_name = COMPUTER_NAME;

	init_shell();

	//加入默认环境变量
	shell_addnew_path("/usr/bin/");

	while (1)
	{
		login();

		char *cmd = malloc(SHELL_CMD_LEN);

		while (session->status == SESSION_STATUS_LOGIN)
		{
			get_current_folder_name(session->current_folder_name);
			printf("[%s@%s %s]: ", session->username, computer_name, session->current_folder_name);
			gets(cmd);
			if (str_compare(SHELL_CMD_EXIT, cmd) == 0)
			{
				break;
			}
			execute_cmd(cmd);
//			execute_cmd("/usr/bin/example_fpu");
		}
		free(cmd);

		logout();
	}

	return 0;
}

void init_shell()
{
	session = malloc(sizeof(s_session));
	session->username = malloc(USER_U_LEN);
	session->home = malloc(USER_H_LEN);
	session->current_path = malloc(SHELL_CMD_LEN);
	session->current_folder_name = malloc(SHELL_CMD_LEN);
	session->PATH = NULL;
	session->username[0] = '\0';
	session->home[0] = '\0';
	session->current_path[0] = '\0';
	session->current_folder_name[0] = '\0';
	session->status = SESSION_STATUS_NOLOGIN;
}

void shell_addnew_path(char *path)
{
	if (session->PATH == NULL)
	{
		session->PATH = malloc(sizeof(s_path));
		session->PATH->path = malloc(SHELL_CMD_LEN);
		session->PATH->next = NULL;
		str_copy(path, session->PATH->path);
		return;
	}

	s_path* p = session->PATH;
	while (p != NULL)
	{
		if (p->next == NULL)
		{
			s_path *pt = malloc(sizeof(s_path));
			pt->path = malloc(SHELL_CMD_LEN);
			pt->next = NULL;
			str_copy(path, pt->path);
			p->next = pt;
			return;
		}
		p = p->next;
	}
}

void login()
{
	char *username = malloc(USER_U_LEN);
	char *passwd = malloc(USER_P_LEN);
	while (session->status == SESSION_STATUS_NOLOGIN)
	{
		printf("LidqOS login: ");
		gets(username);
		char *p = passwd;
		puts("Password: ");
		while ((*p = getchar()) != '\n')
		{
			//putchar('*');
			p++;
		}
		putchar('\n');
		*p = '\0';

		check_passwd(username, passwd);
	}
	free(username);
	free(passwd);
}

void logout()
{
	session->username[0] = '\0';
	session->home[0] = '\0';
	session->status = SESSION_STATUS_NOLOGIN;
}

int check_passwd(char *username, char *passwd)
{
//	str_copy("lidq", session->username);
//	str_copy("/home/lidq/", session->home);
//	session->uid = 1;
//	session->gid = 1;
//	session->status = SESSION_STATUS_LOGIN;
//	return 1;

//临时校验root/123456
	if (str_compare(username, "root") == 0 && str_compare(passwd, "root") == 0)
	{
		str_copy(username, session->username);
		str_copy("/root/", session->home);
		str_copy(session->home, session->current_path);
		session->uid = 0;
		session->gid = 0;
		session->status = SESSION_STATUS_LOGIN;

		printf("Login successful.\n");

		return 1;
	}
	else if (str_compare(username, "lidq") == 0 && str_compare(passwd, "lidq") == 0)
	{
		str_copy(username, session->username);
		str_copy("/home/lidq/", session->home);
		str_copy(session->home, session->current_path);
		session->uid = 1;
		session->gid = 1;
		session->status = SESSION_STATUS_LOGIN;

		printf("Login successful.\n");

		return 1;
	}
	else
	{
		printf("Login incorrect.\n");
	}
	return 0;
}

void repath(char *path, char *current_path, char *full_path)
{
	if (path == NULL)
	{
		return;
	}

	s_stack *sp = stack_init(sizeof(s_folder_name), FOLDE_RNAME_STACK_SIZE);
	s_folder_name *sfn = malloc(sizeof(s_folder_name));
	if (current_path != NULL && path[0] != '/')
	{
		for (int i = 0, j = 0; i < str_len(current_path); i++)
		{
			sfn->folder_name[j] = current_path[i];
			if (current_path[i] == '/')
			{
				sfn->folder_name[j + 1] = '\0';
				stack_push(sp, sfn);
				j = 0;
			}
			else
			{
				j++;
			}
		}
	}

	for (int i = 0, j = 0; i < str_len(path); i++)
	{
		sfn->folder_name[j] = path[i];

		if (path[i] == '/' || path[i + 1] == '\0')
		{
			sfn->folder_name[j + 1] = '\0';
			if (i != 0 && str_compare(sfn->folder_name, "/") == 0)
			{
			}
			else if (str_compare(sfn->folder_name, "./") == 0)
			{
			}
			else if (str_compare(sfn->folder_name, "../") == 0)
			{
				stack_pop(sp, sfn);
				if (stack_count(sp) <= 0)
				{
					sfn->folder_name[0] = '/';
					sfn->folder_name[1] = '\0';
					stack_push(sp, sfn);
				}
				sfn->folder_name[0] = '\0';
			}
			else
			{
				//printf("push %s\n", sfn->folder_name);
				stack_push(sp, sfn);
			}
			j = 0;
		}
		else
		{
			j++;
		}
	}

	s_stack *sp_build = stack_init(sizeof(s_folder_name), FOLDE_RNAME_STACK_SIZE);

	while (stack_count(sp) > 0)
	{
		stack_pop(sp, sfn);
		stack_push(sp_build, sfn);
	}
	int j = 0;
	while (stack_count(sp_build) > 0)
	{
		stack_pop(sp_build, sfn);
		for (int i = 0; i < str_len(sfn->folder_name); i++)
		{
			full_path[j++] = sfn->folder_name[i];
			if (sfn->folder_name[i + 1] == '\0')
			{
				break;
			}
		}
	}
	full_path[j] = '\0';

	free(sfn);
	stack_free(sp_build);
	stack_free(sp);
}

void find_cmd_in_path(char *cmd, char* out)
{
	//以'/'开头: 绝对路径
	if (cmd[0] == '/')
	{
		repath(cmd, NULL, out);
	}
	//不是以'/'开头: 相对路径
	else if ((cmd[0] == '.' && cmd[1] == '/') || (cmd[0] == '.' && cmd[1] == '.' && cmd[2] == '/'))
	{
		repath(cmd, session->current_path, out);
	}
	else
	{
		s_path *p = session->PATH;
		while (p != NULL)
		{
			str_append(p->path, cmd, out);
			FILE *fp = fopen(out, FS_MODE_READ);
			if (fp != NULL)
			{
				fclose(fp);
				return;
			}
			fclose(fp);
			out[0] = '\0';
			p = p->next;
		}
	}
}

void execute_cmd(char *cmd)
{
	if (str_len(cmd) <= 0)
	{
		return;
	}
	char *cmd_file = malloc(SHELL_CMD_LEN);
	char *cmd_param = malloc(SHELL_CMD_LEN);
	char *p = cmd;
	int ty = 0;
	int j = 0, k = 0;
	for (int i = 0; i < str_true_len(p); i++)
	{
		if (p[i] == ' ' && ty == 0)
		{
			ty = 1;
			continue;
		}

		if (ty == 0)
		{
			cmd_file[k] = p[k];
			k++;
		}
		else
		{
			cmd_param[j] = p[i];
			j++;
		}
	}
	cmd_file[k] = '\0';
	cmd_param[j] = '\0';
	char *full_path = malloc(SHELL_CMD_LEN);
	full_path[0] = '\0';
	find_cmd_in_path(cmd_file, full_path);
	FILE *fp = fopen(full_path, FS_MODE_READ);
	if (fp == NULL)
	{
		printf("-bash: \"%s\": command not found.\n", cmd_file);
		free(cmd_param);
		free(full_path);
		free(cmd_file);
		fclose(fp);
		return;
	}

	Elf32_Ehdr *ehdr = malloc(fp->fs.size);
	fread(fp, sizeof(Elf32_Ehdr), (char *) ehdr);
	free(ehdr);
	fclose(fp);
	if (check_elf_file(ehdr))
	{
		install_program(full_path, cmd);
	}
	else
	{
		printf("-bash: \"%s\": is not an executing program.\n", cmd_file);
	}
	free(cmd_param);
	free(full_path);
	free(cmd_file);
}

int check_elf_file(Elf32_Ehdr *ehdr)
{
	int check_passed = 1;
	if (ehdr->e_ident[0] != 0x7f)
	{
		check_passed = 0;
	}
	if (ehdr->e_ident[1] != 0x45)
	{
		check_passed = 0;
	}
	if (ehdr->e_ident[2] != 0x4c)
	{
		check_passed = 0;
	}
	if (ehdr->e_ident[3] != 0x46)
	{
		check_passed = 0;
	}
	if (ehdr->e_ident[4] != 0x01)
	{
		check_passed = 0;
	}
	if (ehdr->e_ident[5] != 0x01)
	{
		check_passed = 0;
	}
	if (ehdr->e_type != 0x01)
	{
		check_passed = 0;
	}
	return check_passed;
}

void install_program(char *path, char *args)
{
	u32 sem_addr = 0;
	int params[0x10];
	params[0] = 0;
	params[1] = (int) path;
	params[2] = (int) args;
	params[3] = (int) session;
	params[4] = (int) &sem_addr;
	__asm__ volatile("int $0x80" :: "a"(params));
	if (sem_addr != 0)
	{
		sem_wait_shell(sem_addr);
		sem_post_shell(sem_addr + sizeof(s_sem));
	}
}

void get_current_folder_name(char *folder_name)
{
	int last_ind = 0;
	//最后一个'/'不算
	for (int i = str_len(session->current_path) - 2; i >= 0; i--)
	{
		if (session->current_path[i] == '/')
		{
			last_ind = i;
			break;
		}
	}
	str_sub(session->current_path, last_ind + 1, str_len(session->current_path) - (last_ind + 1), folder_name);
	if (folder_name[0] == '\0')
	{
		folder_name[0] = '/';
		folder_name[1] = '\0';
	}
}

