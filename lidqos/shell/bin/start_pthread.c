/*
 * start_pthread.c
 *
 *  Created on: May 26, 2015
 *      Author: lidq
 */


extern int pthread_function(void *args);

void start_pthread()
{
	void *args = (void *)0;
	pthread_function(args);

	int params[2];
	//type为1,代表停止线程
	params[0] = 1;
	//请求0x80号中断服务
	__asm__ volatile("int	$0x80" :: "a"(params));
}
