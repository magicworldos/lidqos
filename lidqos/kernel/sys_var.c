#include <kernel/sys_var.h>
#include <kernel/alloc.h>

s_sys_var *sys_var = NULL;

void init_sys_var()
{
	if (sys_var == NULL)
	{
		sys_var = alloc_mm(sizeof(s_sys_var));
	}
}
