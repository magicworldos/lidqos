#include <kernel/sys_var.h>
#include <kernel/alloc.h>

s_sys_var *sys_var = NULL;

void install_sys_var()
{
	if (sys_var == NULL)
	{
		sys_var = alloc_mm(sizeof(s_sys_var));
		sys_var->pt_count = 0;
	}
}

void install_pts(int pt_count, void *pts)
{
	mmcopy(pts, sys_var->pts, sizeof(sys_var->pts));
	sys_var->pt_count = pt_count;
}
