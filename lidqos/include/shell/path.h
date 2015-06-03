/*
 * path.h
 *
 *  Created on: Jun 3, 2015
 *      Author: lidq
 */

#ifndef _INCLUDE_SHELL_PATH_H_
#define _INCLUDE_SHELL_PATH_H_

#include <kernel/typedef.h>
#include <kernel/sys_var.h>
#include <kernel/string.h>
#include <shell/stack.h>


typedef struct
{
	char folder_name[SHELL_CMD_LEN];
} s_folder_name;

#define FOLDE_RNAME_STACK_SIZE	(0x200)

void repath(char *path, char *current_path, char *full_path);

#endif /* INCLUDE_SHELL_PATH_H_ */
