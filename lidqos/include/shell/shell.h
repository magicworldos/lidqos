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

int main(int argc, char **args);

void execute_cmd(char *cmd);

void install_program(char *path, char *args);

#endif /* INCLUDE_SHELL_SHELL_H_ */
