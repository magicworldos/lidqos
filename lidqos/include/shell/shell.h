/*
 * shell.h
 *
 *  Created on: May 29, 2015
 *      Author: lidq
 */

#ifndef _INCLUDE_SHELL_SHELL_H_
#define _INCLUDE_SHELL_SHELL_H_

#include <shell/stdio.h>
#include <shell/stdlib.h>
#include <shell/unistd.h>
#include <shell/semaphore.h>
#include <shell/stdio.h>
#include <shell/fs.h>
#include <shell/shell.h>
#include <shell/stack.h>

//计算机名，以后是要从配置文件里读的
#define COMPUTER_NAME				"lidq-os"

typedef struct
{
	char folder_name[0x200];
} s_folder_name;

#define FOLDE_RNAME_STACK_SIZE	(0x200)

int main(int argc, char **args);

void init_shell();

void shell_addnew_path(char *path);

void login();

void logout();

int check_passwd(char *username, char *passwd);

void repath(char *path, char *current_path, char *full_path);

void find_cmd_in_path(char *cmd, char* out);

void execute_cmd(char *cmd);

void install_program(char *path, char *args);

void get_current_folder_name(char *folder_name);

#endif /* INCLUDE_SHELL_SHELL_H_ */