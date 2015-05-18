/*
 * string.h
 * Copyright (C) Feb 10, 2014 by lidq
 *
 * 字符串操作头文件
 */

#ifndef _STRING_H_
#define _STRING_H_

/*
 * str_len : 计算字符串长度，不包括'\0'
 *  - char *str : 字符串
 * return : int字符串长度长度
 */
int str_len(char *str);

/*
 * str_true_len : 计算字符串真实长度，包括'\0'
 *  - char *str : 字符串
 * return : int字符串长度长度
 */
int str_true_len(char *str);

/*
 * str_compare : 字符串比较
 *  - char *str : 字符串一
 *  - char *str2 : 字符串二
 * return : int 0相等，1大于，2小于
 */
int str_compare(char *str, char *str2);

/*
 * str_copy : 拷贝字符串
 *  - char *from : 源地址
 *  - char *to : 目的地址
 * return : void
 */
void str_copy(char *from, char *to);

/*
 * str_split_count : 分割字符串分割数
 *  - char *str : 字符串
 *  - char ch : 分割符
 * return : 分割数
 */
int str_split_count(char *str, char ch);

/*
 * str_split : 分割字符串
 *  - char *str : 分割字符串
 *  - char ch : 分割符
 *  - char **str_list : 分割后的字符串数组
 * return : void
 */
void str_split(char *str, char ch, char **str_list);

/*
 * str_append : 字符串连接
 *  - char *str : 字符串一
 *  - char *str2 : 字符串二
 *  - char *result : 连接后的字符串
 * return : void
 */
void str_append(char *str, char *str2, char *result);

void str_sub(char *str, int start, int size, char *out);

int str_indexof(char *str, char ch);

int str_last_indexof(char *str, char ch);

#endif
