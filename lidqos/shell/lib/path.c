/*
 * path.c
 *
 *  Created on: Jun 3, 2015
 *      Author: lidq
 */

#include <shell/path.h>

/*
 * 重设路径
 */
void repath(char *path, char *current_path, char *full_path)
{
	//如果路径为空则返回
	if (path == NULL)
	{
		return;
	}

	//初始化一个栈，用于替换.和..
	s_stack *sp = stack_init(sizeof(s_folder_name), FOLDE_RNAME_STACK_SIZE);
	//临时变量，用于存放目录
	s_folder_name *sfn = malloc(sizeof(s_folder_name));
	//如果不是以/开头
	if (current_path != NULL && path[0] != '/')
	{
		//逐个取得路径字符
		for (int i = 0, j = 0; i < str_len(current_path); i++)
		{
			//复制路径
			sfn->folder_name[j] = current_path[i];
			//遇到/符号开始处理之前取得的字符，作为一个可用的目录名
			if (current_path[i] == '/')
			{
				//字符串结束
				sfn->folder_name[j + 1] = '\0';
				//入栈
				stack_push(sp, sfn);
				//重新接收新的目录名
				j = 0;
			}
			else
			{
				j++;
			}
		}
	}

	//处理目录
	for (int i = 0, j = 0; i < str_len(path); i++)
	{
		sfn->folder_name[j] = path[i];
		//取得目录名
		if (path[i] == '/' || path[i + 1] == '\0')
		{
			//字符串结尾
			sfn->folder_name[j + 1] = '\0';
			//普通目录结尾符
			if (i != 0 && str_compare(sfn->folder_name, "/") == 0)
			{
			}
			//当前目录结尾符
			else if (str_compare(sfn->folder_name, "./") == 0)
			{
			}
			//父级目录结尾符
			else if (str_compare(sfn->folder_name, "../") == 0)
			{
				//出栈
				stack_pop(sp, sfn);
				//如果栈为空
				if (stack_count(sp) <= 0)
				{
					//将/入栈
					sfn->folder_name[0] = '/';
					sfn->folder_name[1] = '\0';
					//将/入栈
					stack_push(sp, sfn);
				}
				//字符串结束
				sfn->folder_name[0] = '\0';
			}
			else
			{
				stack_push(sp, sfn);
			}
			j = 0;
		}
		else
		{
			j++;
		}
	}

	//初始化构建最终路径的栈
	s_stack *sp_build = stack_init(sizeof(s_folder_name), FOLDE_RNAME_STACK_SIZE);

	//如果临时栈中有值，则出栈
	while (stack_count(sp) > 0)
	{
		//出栈
		stack_pop(sp, sfn);
		//入栈到新栈中
		stack_push(sp_build, sfn);
	}
	int j = 0;
	//将新栈中的所以目录名出栈
	while (stack_count(sp_build) > 0)
	{
		//出栈
		stack_pop(sp_build, sfn);
		//设定目录名
		for (int i = 0; i < str_len(sfn->folder_name); i++)
		{
			//复制目录字符串
			full_path[j++] = sfn->folder_name[i];
			if (sfn->folder_name[i + 1] == '\0')
			{
				break;
			}
		}
	}
	//字符串结尾符
	full_path[j] = '\0';

	//释放内存
	free(sfn);
	stack_free(sp_build);
	stack_free(sp);
}
