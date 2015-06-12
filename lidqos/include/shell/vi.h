/*
 * vi.h
 *
 *  Created on: Jun 12, 2015
 *      Author: lidq
 */

#ifndef _INCLUDE_SHELL_VI_H_
#define _INCLUDE_SHELL_VI_H_

#include <kernel/typedef.h>

typedef struct
{
	u8 *line;
	void *next;
} s_vdata;

int main(int argc, char **args);

void edit_file(char *path, void *data, int size);

#endif /* INCLUDE_SHELL_VI_H_ */
