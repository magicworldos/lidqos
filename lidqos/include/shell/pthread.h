/*
 * pthread.h
 *
 *  Created on: May 25, 2015
 *      Author: lidq
 */

#ifndef _INCLUDE_SHELL_PTHREAD_H_
#define _INCLUDE_SHELL_PTHREAD_H_

#include <kernel/typedef.h>

void pthread_create(s_pthread *p, void *function, void *args);

#endif /* INCLUDE_SHELL_PTHREAD_H_ */
