/*
 * main.c
 *
 *  Created on: Mar 26, 2015
 *      Author: lidq
 */

#include <boot/code16.h>
#include <boot/main.h>

int main(int argc, char **args)
{

	//测试，如果显示字符b说明main已经执行
	//	__asm__(
	//			"movw	$0xb800, %ax	\n\t"
	//			"movw	%ax, %es		\n\t"
	//			"movw	$0x2, %ax		\n\t"
	//			"movw	%ax, %di		\n\t"
	//			"movw	$0x0762, %ax	\n\t"
	//			"movw	%ax, %es:(%di)	\n\t"
	//	);

	for (;;)
	{

	}

	return 0;
}
