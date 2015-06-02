/*
 * start_main.h
 * Copyright (C) Feb 10, 2014 by lidq
 *
 * 启动程序外壳载入头文件
 */

#ifndef _START_MAIN_H_
#define _START_MAIN_H_

#include <kernel/typedef.h>
#include <kernel/sys_var.h>
#include <kernel/string.h>
#include <shell/stdlib.h>
#include <shell/semaphore.h>

/*
 * start_main : 启动函数
 * return : void
 */
void start_main();

#endif /* START_MAIN_H_ */
