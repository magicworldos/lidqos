/*
 * string.c
 * Copyright (C) Feb 10, 2014 by lidq
 *
 * 字符串操作
 */

#ifndef _STRING_C_
#define _STRING_C_

#include <kernel/string.h>

/*
 * str_len : 计算字符串长度，不包括'\0'
 *  - char *str : 字符串
 * return : int字符串长度长度
 */
int str_len(char *str)
{
	int len = 0;
	char * pCh = str;
	while (*pCh != '\0')
	{

		pCh++;
		len++;
	}
	return len;
}

/*
 * str_true_len : 计算字符串真实长度，包括'\0'
 *  - char *str : 字符串
 * return : int字符串长度长度
 */
int str_true_len(char *str)
{
	return str_len(str) + 1;
}

/*
 * str_compare : 字符串比较
 *  - char *str : 字符串一
 *  - char *str2 : 字符串二
 * return : int 0相等，1大于，2小于
 */
int str_compare(char *str, char *str2)
{
	int ret = 0;
	while (*str != '\0' && *str2 != '\0')
	{
		if (*str < *str2)
		{
			return -1;
		}
		else if (*str > *str2)
		{
			return 1;
		}
		str++;
		str2++;
	}
	int len = str_len(str);
	int len2 = str_len(str2);
	if (len == len2)
	{
		return 0;
	}
	else if (len < len2)
	{
		return -1;
	}
	else
	{
		return 1;
	}
}

/*
 * str_copy : 拷贝字符串
 *  - char *from : 源地址
 *  - char *to : 目的地址
 * return : void
 */
void str_copy(char *from, char *to)
{
	while (*from != '\0')
	{
		*to++ = *from++;
	}
	*to = '\0';
}

/*
 * str_split_count : 分割字符串分割数
 *  - char *str : 字符串
 *  - char ch : 分割符
 * return : 分割数
 */
int str_split_count(char *str, char ch)
{
	int count = 0;
	while (*str != '\0')
	{
		if (*str == ch)
		{
			count++;
		}
		str++;
	}
	return ++count;
}

/*
 * str_split : 分割字符串
 *  - char *str : 分割字符串
 *  - char ch : 分割符
 *  - char **str_list : 分割后的字符串数组
 * return : void
 */
void str_split(char *str, char ch, char **str_list)
{
	int x = 0;
	int y = 0;
	int i;
	for (i = 0; str[i] != '\0'; ++i)
	{
		if (str[i] != ch)
		{
			str_list[x][y] = str[i];
			y++;
		}
		else
		{
			str_list[x][y] = '\0';
			x++;
			y = 0;
		}
	}
	str_list[x][y] = '\0';
}

/*
 * str_append : 字符串连接
 *  - char *str : 字符串一
 *  - char *str2 : 字符串二
 *  - char *result : 连接后的字符串
 * return : void
 */
void str_append(char *str, char *str2, char *result)
{
	while (*str != '\0')
	{
		*result++ = *str++;
	}
	while (*str2 != '\0')
	{
		*result++ = *str2++;
	}
	*result = '\0';
}

void str_sub(char *str, int start, int size, char *out)
{
	int j = 0;
	for (int i = start; str[i] != '\0' && j < size; i++, j++)
	{
		out[j] = str[i];
	}
	out[j] = '\0';
}

int str_indexof(char *str, char ch)
{
	for (int i = 0; str[i] != '\0'; i++)
	{
		if (str[i] == ch)
		{
			return i;
		}
	}
	return -1;
}

int str_last_indexof(char *str, char ch)
{
	for (int i = str_len(str) - 1; i >= 0; i--)
	{
		if (str[i] == ch)
		{
			return i;
		}
	}
	return -1;
}
#endif
