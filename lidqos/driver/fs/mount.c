/*
 * mount.c
 *
 *  Created on: Feb 12, 2015
 *      Author: lidq
 */

#include <kernel/mount.h>

s_tree *root = NULL;
int mount_next_id = 0;

/*
 * 载入根节点
 */
void mount_root()
{
	//默认为系统光盘
	s_mount *mount = alloc_mm(sizeof(s_mount));
	mount->dev_id = HD_DEV_ID_OSISO;
	mount->path[0] = '/';
	mount->path[1] = '\0';
	root = alloc_mm(sizeof(s_tree));
	tree_init_node(root, mount_next_id++);
	root->node = mount;
}

u32 find_dev_id_fullpath(char *full_path, char *out_path)
{
	u32 dev_id = 0;
	int ind = 0;
	char *path = alloc_mm(0x200);

	for (int i = 0; full_path[i] != '\0'; i++)
	{
		path[i] = full_path[i];

		if (path[i] == '/')
		{
			path[i + 1] = '\0';
			u32 dev_id_temp = find_dev_id(root, path);
			if (dev_id_temp != 0)
			{
				dev_id = dev_id_temp;
				ind = i;
			}
		}
	}

	int i = 0;
	for (i = 0; full_path[i + ind] != '\0'; i++)
	{
		out_path[i] = full_path[i + ind];
	}
	out_path[i] = '\0';

	free_mm(path, 0x200);
	return dev_id;
}

u32 find_dev_id(s_tree *tree, char *path)
{
	s_tree *p = tree;
	while (p != NULL)
	{
		s_mount *mp = (s_mount *) p->node;
		if (str_compare(path, mp->path) == 0)
		{
			return mp->dev_id;
		}

		if (p->child != NULL)
		{
			return find_dev_id(p->child, path);
		}
		p = p->next;
	}

	return 0;
}

int find_mount_id_fullpath(char *full_path)
{
	int mount_id = 0;
	char *path = alloc_mm(0x200);
	for (int i = 0; full_path[i] != '\0'; i++)
	{
		path[i] = full_path[i];

		if (path[i] == '/')
		{
			path[i + 1] = '\0';
			int mount_id_temp = find_mount_id(root, path);
			if (mount_id_temp != 0)
			{
				mount_id = mount_id_temp;
			}
		}
	}
	free_mm(path, 0x200);
	return mount_id;
}

int find_mount_id(s_tree *tree, char *path)
{
	s_tree *p = tree;
	while (p != NULL)
	{
		s_mount *mp = (s_mount *) p->node;
		if (str_compare(path, mp->path) == 0)
		{
			return p->id;
		}

		if (p->child != NULL)
		{
			return find_mount_id(p->child, path);
		}
		p = p->next;
	}

	return 0;
}

void mount_hda(s_pt *pts, char *mount_point)
{
	s_mount *mount = alloc_mm(sizeof(s_mount));
	mount->dev_id = pts->device;
	str_copy(mount_point, mount->path);

	s_tree *mp = alloc_mm(sizeof(s_tree));
	tree_init_node(mp, mount_next_id++);
	mp->node = mount;
	int mount_id = find_mount_id_fullpath(mount_point);
	tree_insert_node(root, mount_id, mp);
}
