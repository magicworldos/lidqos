/*
 * shell.c
 *
 *  Created on: Nov 26, 2014
 *      Author: lidq
 */

#include <shell/shell.h>

//用户登录信息
s_session *session = NULL;

int main(int argc, char **args)
{
	//设置计算机名（以后要从/etc/hostname里读入）
	char *computer_name = COMPUTER_NAME;

	//初始化shell相关变量
	init_shell();

	//加入默认环境变量
	shell_init_path("/usr/bin/");

	//始终循环
	while (1)
	{
		//登录
		login();

		//申请接收用户的命令
		char *cmd = malloc(SHELL_CMD_LEN);

		//如果尚已登录成功
		while (session->status == SESSION_STATUS_LOGIN)
		{
			//取得当前工作目录
			get_current_folder_name(session->current_folder_name);
			//显示命令提示
			printf("[%s@%s %s]: ", session->username, computer_name, session->current_folder_name);
			//接收用户命令
			gets(cmd);
			//如果用户输入了exit，则退出
			if (str_compare(SHELL_CMD_EXIT, cmd) == 0)
			{
				break;
			}
			//执行用户输入的命令
			execute_cmd(cmd);
		}

		//释放命令内存
		free(cmd);

		//退出
		logout();
	}

	return 0;
}

/*
 * 初始化相关变量
 */
void init_shell()
{
	//用于存放用户登录信息的session
	session = malloc(sizeof(s_session));
	//用户名
	session->username = malloc(USER_U_LEN);
	//主目录
	session->home = malloc(USER_H_LEN);
	//当前工作目录
	session->current_path = malloc(SHELL_CMD_LEN);
	//当前文件夹名
	session->current_folder_name = malloc(SHELL_CMD_LEN);
	//环境变量
	session->PATH = malloc(SHELL_CMD_LEN);
	//初始化各个变量
	session->username[0] = '\0';
	session->home[0] = '\0';
	session->current_path[0] = '\0';
	session->current_folder_name[0] = '\0';
	session->PATH[0] = '\0';
	//设定用户登录状态为未登录
	session->status = SESSION_STATUS_NOLOGIN;

	int params[2];
	params[0] = 4;
	params[1] = (int) session;
	__asm__ volatile("int $0x80" :: "a"(params));
}

/*
 * 初始化环境变量
 */
void shell_init_path(char *path)
{
	//复制path到环境变量中
	str_copy(path, session->PATH);
}

/*
 * 用户登录
 */
void login()
{
	//用户名
	char *username = malloc(USER_U_LEN);
	//密码
	char *passwd = malloc(USER_P_LEN);
	//如果未登录，则一直提示需要先登录
	while (session->status == SESSION_STATUS_NOLOGIN)
	{
		//显示登录信息
		printf("LidqOS login: ");
		//接收用户名
		gets(username);
		//显示输入密码
		puts("Password: ");
		//取得密码
		char *p = passwd;
		//接收密码字符
		while ((*p = getchar()) != '\n')
		{
			//不显示密码内存或*号
			//putchar('*');
			p++;
		}
		putchar('\n');
		*p = '\0';
		//校验用户名与密码是否有效
		check_passwd(username, passwd);
	}
	free(username);
	free(passwd);
}

/*
 * 退出
 */
void logout()
{
	//清空用户名
	session->username[0] = '\0';
	//清空主目录
	session->home[0] = '\0';
	//设定为未登录状态
	session->status = SESSION_STATUS_NOLOGIN;
}

/*
 * 校验用户名与密码是否有效
 */
int check_passwd(char *username, char *passwd)
{
	//校验root
	if (str_compare(username, "root") == 0 && str_compare(passwd, "root") == 0)
	{
		//设置session中的用户名
		str_copy(username, session->username);
		//主目录
		str_copy("/root/", session->home);
		//当前工作目录
		str_copy(session->home, session->current_path);
		//用户id
		session->uid = 0;
		//用户组id
		session->gid = 0;
		//设定为已登录
		session->status = SESSION_STATUS_LOGIN;
		//显示登录成功
		printf("Login successful.\n");
		//初始化session到pcb
		int params[2];
		params[0] = 4;
		params[1] = (int) session;
		__asm__ volatile("int $0x80" :: "a"(params));
		//返回成功
		return 1;
	}
	//校验lidq
	else if (str_compare(username, "lidq") == 0 && str_compare(passwd, "lidq") == 0)
	{
		//设置session中的用户名
		str_copy(username, session->username);
		//主目录
		str_copy("/home/lidq/", session->home);
		//当前工作目录
		str_copy(session->home, session->current_path);
		//用户id
		session->uid = 1;
		//用户组id
		session->gid = 1;
		//设定为已登录
		session->status = SESSION_STATUS_LOGIN;
		//显示登录成功
		printf("Login successful.\n");
		//初始化session到pcb
		int params[2];
		params[0] = 4;
		params[1] = (int) session;
		__asm__ volatile("int $0x80" :: "a"(params));
		//返回成功
		return 1;
	}
	//校验失败
	else
	{
		//显示登录被拒绝
		printf("Login incorrect.\n");
	}
	//返回失败
	return 0;
}

/*
 * 查找命令的完整路径
 */
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
	//使用环境变量
	else
	{
		str_append(session->PATH, cmd, out);
	}
}

/*
 * 执行命令
 */
void execute_cmd(char *cmd)
{
	//如果命令为空
	if (str_len(cmd) <= 0)
	{
		//返回
		return;
	}

	char *cmd_file = malloc(SHELL_CMD_LEN);
	char *cmd_param = malloc(SHELL_CMD_LEN);
	char *full_path = malloc(SHELL_CMD_LEN);

	//将命令分割成“命令+参数”两个字符串
	char *p = cmd;
	int ty = 0;
	int j = 0, k = 0;
	for (int i = 0; i < str_true_len(p); i++)
	{
		//发现空格，分割
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
	//设定字符串结尾
	cmd_file[k] = '\0';
	cmd_param[j] = '\0';
	full_path[0] = '\0';

	//找到命令的完整路径
	find_cmd_in_path(cmd_file, full_path);

	//安装程序
	install_program(full_path, cmd);

	//释放内存
	free(cmd_param);
	free(full_path);
	free(cmd_file);
}

/*
 * 安装程序
 */
void install_program(char *path, char *args)
{
	//返回状态
	int status = 0;
	//程序运行信号量
	u32 sem_addr = 0;

	int params[6];
	params[0] = 0;
	params[1] = (int) path;
	params[2] = (int) args;
	params[3] = (int) 1;
	params[4] = (int) &sem_addr;
	params[5] = (int) &status;
	__asm__ volatile("int $0x80" :: "a"(params));

	//如果状态为0,说明安装程序成功
	if (status == 0)
	{
		//等待程序运行结束信号量
		sem_wait_shell(sem_addr);
		//发送release程序信号量
		sem_post_shell(sem_addr + sizeof(s_sem));
	}
	//如果状态为1,说明没有这个文件
	else if (status == 1)
	{
		printf("-bash: \"%s\": command not found.\n", path);
	}
	//如果状态为2,说明这不是一个可执行文件
	else if (status == 2)
	{
		printf("-bash: \"%s\": is not an executing program.\n", path);
	}
	//其它：为程序运行申请内存是失败
	else
	{
		printf("-bash: \"%s\": alloc memory error.\n", path);
	}
}

/*
 * 取得当前工作路径的文件夹名
 */
void get_current_folder_name(char *folder_name)
{
	//重新取得当前目录
	int params[2];
	params[0] = 9;
	params[1] = (int) session->current_path;
	__asm__ volatile("int $0x80" :: "a"(params));

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
	//截取字符串
	str_sub(session->current_path, last_ind + 1, str_len(session->current_path) - (last_ind + 1), folder_name);
	if (folder_name[0] == '\0')
	{
		folder_name[0] = '/';
		folder_name[1] = '\0';
	}
}

