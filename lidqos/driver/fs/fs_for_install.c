/*
 * fs.c
 * Copyright (C) Feb 10, 2014 by lidq
 *
 * 文件系统:
 *  - 构建文件系统，为其它程序的文件操作提供相关功能函数
 */

#include <kernel/typedef.h>
#include <kernel/hd_for_install.h>
#include <kernel/fs_for_install.h>
#include <kernel/mm.h>
#include <kernel/string.h>
#include <kernel/alloc.h>

//int mount_id = 0;
//默认操作设备为系统光盘，主IDE的从盘
u32 hd_dev_id = HD_DEV_ID_OSISO;

/*
 * fs_empty_data : 清空fs块的内容
 *  - void *fs : fs文件块
 * return : void
 */
void fs_empty_data(void *fs)
{
	//清空fs块的内容
	u8 *data = (u8*) fs;
	for (int i = 0; i < FS_DATA_COUNT; ++i)
	{
		//置为0x0
		data[i] = 0x0;
	}
}

/*
 * fs_find_path : 根据路径查找fs文件块
 *  - char *path_name : 待查找路径
 *  - s_fs **fs : 文件块指针的地址
 * return : void
 */
void fs_find_path(char *path_name, s_fs **fs)
{
	//临时保存文件夹名称
	char folder_name[FS_NAME_LENGTH];
	//父级文件块编号
	u32 parent_no = 0;
	for (int i = 0, j = 0; path_name[i] != '\0'; ++i)
	{
		//以'/'为分割符取得文件夹名称
		if (path_name[i] != '/')
		{
			folder_name[j++] = path_name[i];
		}
		else
		{
			//取得文件夹名称
			folder_name[j++] = '/';
			folder_name[j++] = '\0';
			j = 0;
			//根据父级文件块编号查找其下的直属子文件
			int child_no = fs_find_sub_fs(parent_no, folder_name);
			//没有找到此文件
			if (child_no == 0)
			{
				//返回空
				*fs = NULL;
				return;
			}
			//找到子文件块，将其设定为下一级父文件块编号
			parent_no = child_no;
		}
	}
	//根据文件块编号读取文件块内容，并写入文件块指针所指向的地址
	read_block(hd_dev_id, parent_no, *fs);
	return;
}

/*
 * fs_create_path : 根据路径创建多级文件目录
 *  - char *path_name : 待创建的路径
 * return : void
 */
void fs_create_path(char *path_name, u32 uid, u32 gid, u32 mode)
{
	//临时保存文件夹名称
	char *folder_name = alloc_mm(FS_NAME_LENGTH);
	//父级文件块编号
	u32 parent_no = 0;
	for (int i = 0, j = 0; path_name[i] != '\0'; ++i)
	{
		//以'/'为分割符取得文件夹名称
		if (path_name[i] != '/')
		{
			folder_name[j++] = path_name[i];
		}
		else
		{
			folder_name[j++] = '/';
			folder_name[j++] = '\0';
			j = 0;

			//根据父级文件块编号查找其下的直属子文件
			int child_no = fs_find_sub_fs(parent_no, folder_name);
			//没有找到此文件
			if (child_no == 0)
			{
				//创建此文件夹
				child_no = fs_create_fs(parent_no, folder_name, uid, gid, mode);
			}
			//找到子文件块，将其设定为下一级父文件块编号
			parent_no = child_no;
		}
	}
	free_mm(folder_name, FS_NAME_LENGTH);
}

void fs_find_sno(u32 s_no, s_fs **fs)
{
	//读入文件块内容到fs地址
	read_block(hd_dev_id, s_no, *fs);
}

/*
 * fs_find_sub_fs : 根据父级文件块编号和子文件夹名称查找其编号
 *  - u32 parent_no : 父级文件块编号
 *  - char *folder_name : 子文件夹名称
 * return : u32 子文件夹编号，0代表查找失败
 */
u32 fs_find_sub_fs(u32 parent_no, char *folder_name)
{
	//临时文件块变量
	s_fs *fs = alloc_mm(sizeof(s_fs));

	//如果待查找的是根目录"/"
	if (parent_no == 0 && str_compare("/", folder_name) == 0)
	{
		//设定读取条件编号为0，
		fs->dot = 0;
		//取得根目录fs内容
		load_root(hd_dev_id, fs);
		//返回根目录编号，此编号不为0
		u32 sno = fs->dot;
		free_mm(fs, sizeof(s_fs));
		return sno;
	}

	//读入文件块内容到fs地址
	read_block(hd_dev_id, parent_no, fs);

	if (str_compare(folder_name, "./") == 0)
	{
		u32 sno = fs->dot;
		free_mm(fs, sizeof(s_fs));
		return sno;
	}
	else if (str_compare(folder_name, "../") == 0)
	{
		u32 sno = fs->dotdot;
		free_mm(fs, sizeof(s_fs));
		return sno;
	}

	//如果fs的子项地址为0，说明它没有任何子项
	if (fs->address == 0)
	{
		//返回0
		free_mm(fs, sizeof(s_fs));
		return 0;
	}

	//读入一级地址项内容
	s_fs *fst_addrs = alloc_mm(sizeof(s_fs));
	read_block(hd_dev_id, fs->address, fst_addrs);
	//循环一级地址列表
	for (int i = 0; i < FS_ADDR_COUNT; ++i)
	{
		if (fst_addrs->addr_no[i] != 0)
		{
			//读入二级地址项内容
			s_fs *sec_addrs = alloc_mm(sizeof(s_fs));
			read_block(hd_dev_id, fst_addrs->addr_no[i], sec_addrs);
			for (int j = 0; j < FS_ADDR_COUNT; ++j)
			{
				if (sec_addrs->addr_no[j] != 0)
				{
					//读入三级地址项内容
					s_fs *thrid_addrs = alloc_mm(sizeof(s_fs));
					read_block(hd_dev_id, sec_addrs->addr_no[j], thrid_addrs);
					for (int k = 0; k < FS_ADDR_COUNT; ++k)
					{
						if (thrid_addrs->addr_no[k] != 0)
						{
							//读入子文件块地址项内容
							s_fs *sub_folder = alloc_mm(sizeof(s_fs));
							read_block(hd_dev_id, thrid_addrs->addr_no[k], sub_folder);
							//如果子文件块名称和待查找文件名称相同则说明已找到
							if (str_compare(sub_folder->name, folder_name) == 0)
							{
								//返回已找到的文件块编号
								u32 sno = sub_folder->dot;
								free_mm(sub_folder, sizeof(s_fs));
								free_mm(thrid_addrs, sizeof(s_fs));
								free_mm(sec_addrs, sizeof(s_fs));
								free_mm(fst_addrs, sizeof(s_fs));
								free_mm(fs, sizeof(s_fs));
								return sno;
							}
							free_mm(sub_folder, sizeof(s_fs));
						}
					}
					free_mm(thrid_addrs, sizeof(s_fs));
				}
			}
			free_mm(sec_addrs, sizeof(s_fs));
		}
	}
	free_mm(fst_addrs, sizeof(s_fs));
	free_mm(fs, sizeof(s_fs));
	return 0;
}

/*
 * fs_create_fs : 根据父级文件编号创建子文件（夹）
 *  - u32 parent_no : 低级文件块编号
 *  - char *fs_name : 待创建的文件（夹）名称
 *  - u32 fs_mode : FS_FOLDER_MODE为文件夹，FS_FILE_MODE为文件
 * return : u32 返回新创建的子文件（夹—）编号，0代表创建失败
 */
u32 fs_create_fs(u32 parent_no, char *fs_name, u32 uid, u32 gid, u32 mode)
{
	//根据父级编号读入文件块
	s_fs *fs_parent = alloc_mm(sizeof(s_fs));
	s_fs *fs = alloc_mm(sizeof(s_fs));
	read_block(hd_dev_id, parent_no, fs_parent);
	//清空文件块内容
	fs_empty_data(fs);
	fs->owner = uid;
	fs->group = gid;
	//文件模式
	fs->mode = mode;
	//申请一块未使用的文件块，给本文件块
	fs->dot = alloc_sec(hd_dev_id);
	if (fs->dot == 0)
	{
		free_mm(fs, sizeof(s_fs));
		free_mm(fs_parent, sizeof(s_fs));

		return 0;
	}
	//设定父级文件块编号
	fs->dotdot = fs_parent->dot;
	//root
	if (fs_parent->is_root)
	{
		fs->root = fs_parent->dot;
	}
	else
	{
		fs->root = fs_parent->root;
	}
	//文件大小
	fs->size = 0;
	//设定文件名称
	str_copy(fs_name, fs->name);
	//定入此新的文件块
	write_block(hd_dev_id, fs->dot, fs);

	/*
	 * 以下是将此文件块挂入其父级文件块中
	 */

	//父级子项地址
	s_fs *fst_addrs = alloc_mm(sizeof(s_fs));
	if (fs_parent->address == 0)
	{
		//创建一级磁盘指针
		fs_parent->address = alloc_sec(hd_dev_id);
		fs_empty_data(fst_addrs);
		//保存地址项
		write_block(hd_dev_id, fs_parent->address, fst_addrs);
		//保存父级文件块
		write_block(hd_dev_id, fs_parent->dot, fs_parent);
	}

	//一级地址列表
	read_block(hd_dev_id, fs_parent->address, fst_addrs);
	for (int i = 0; i < FS_ADDR_COUNT; ++i)
	{
		s_fs *sec_addrs = alloc_mm(sizeof(s_fs));
		if (fst_addrs->addr_no[i] == 0)
		{
			//创建二级磁盘指针
			fst_addrs->addr_no[i] = alloc_sec(hd_dev_id);
			fs_empty_data(sec_addrs);
			//保存地址项
			write_block(hd_dev_id, fst_addrs->addr_no[i], sec_addrs);
			//保存父级文件块
			write_block(hd_dev_id, fs_parent->address, fst_addrs);
		}

		//二级地址列表
		read_block(hd_dev_id, fst_addrs->addr_no[i], sec_addrs);
		for (int j = 0; j < FS_ADDR_COUNT; ++j)
		{
			s_fs *thrid_addrs = alloc_mm(sizeof(s_fs));
			if (sec_addrs->addr_no[j] == 0)
			{
				//创建三级磁盘指针
				sec_addrs->addr_no[j] = alloc_sec(hd_dev_id);
				fs_empty_data(thrid_addrs);
				//保存地址项
				write_block(hd_dev_id, sec_addrs->addr_no[j], thrid_addrs);
				//保存父级文件块
				write_block(hd_dev_id, fst_addrs->addr_no[i], sec_addrs);
			}

			//三级地址列表
			read_block(hd_dev_id, sec_addrs->addr_no[j], thrid_addrs);
			for (int k = 0; k < FS_ADDR_COUNT; ++k)
			{
				if (thrid_addrs->addr_no[k] == 0)
				{
					//挂入文件块
					thrid_addrs->addr_no[k] = fs->dot;
					//保存三级文件块内容
					write_block(hd_dev_id, sec_addrs->addr_no[j], thrid_addrs);
					//返回新的文件块编号
					u32 sno = fs->dot;
					free_mm(thrid_addrs, sizeof(s_fs));
					free_mm(sec_addrs, sizeof(s_fs));
					free_mm(fst_addrs, sizeof(s_fs));
					free_mm(fs, sizeof(s_fs));
					free_mm(fs_parent, sizeof(s_fs));
					return sno;
				}
			}
			free_mm(thrid_addrs, sizeof(s_fs));
		}
		free_mm(sec_addrs, sizeof(s_fs));
	}
	free_mm(fst_addrs, sizeof(s_fs));
	free_mm(fs, sizeof(s_fs));
	free_mm(fs_parent, sizeof(s_fs));

	//创建失败，返回0
	return 0;
}

/*
 * fs_create_file : 创建文件
 *  - char *file_name : 文件名
 * return : int -1代表失败
 */
int fs_create_file(char *file_name, u32 uid, u32 gid, u32 mode)
{
	//文件名临时变量
	char *fs_name = alloc_mm(FS_NAME_LENGTH);
	//父级文件块编号
	u32 parent_no = 0;
	for (int i = 0, j = 0; file_name[i] != '\0'; ++i)
	{
		//以'/'为分割符取得文件夹名称
		if (file_name[i] != '/')
		{
			fs_name[j++] = file_name[i];
		}
		else
		{
			//取得文件夹名称
			fs_name[j++] = '/';
			fs_name[j++] = '\0';
			j = 0;

			//根据父级文件块编号查找其下的直属子文件
			int child_no = fs_find_sub_fs(parent_no, fs_name);
			if (child_no == 0)
			{
				//创建文件
				child_no = fs_create_fs(parent_no, fs_name, uid, gid, mode);
			}
			//找到子文件块，将其设定为下一级父文件块编号
			parent_no = child_no;
		}
		if (file_name[i + 1] == '\0')
		{
			fs_name[j++] = '\0';
			//创建文件
			fs_create_fs(parent_no, fs_name, uid, gid, mode);
		}
	}
	free_mm(fs_name, FS_NAME_LENGTH);
	return -1;
}

/*
 * f_create : 创建文件
 *  - char *file_name : 文件名
 * return : int -1代表失败
 */
int f_create(char *file_name, u32 uid, u32 gid, u32 mode)
{
	return fs_create_file(file_name, uid, gid, mode);
}

/*
 * f_open : 打开文件
 *  - char *file_name : 完整路径文件名
 *  - int fs_mode : FS_MODE_WRITE为写模式，FS_MODE_READ为读模式
 * return : s_file*文件结构指针
 */
s_file* f_open(char *file_name, int fs_mode)
{
	if (str_len(file_name) == 0)
	{
		return NULL;
	}

	//文件名称，临时变量
	char *fs_name = alloc_mm(FS_NAME_LENGTH);
	u32 parent_no = 0;
	int child_no = 0;
	for (int i = 0, j = 0; file_name[i] != '\0'; ++i)
	{
		//取得文件目录名称
		if (file_name[i] != '/')
		{
			fs_name[j++] = file_name[i];
		}
		else
		{
			//根据目录名称查找子目录
			fs_name[j++] = '/';
			fs_name[j++] = '\0';
			j = 0;
			child_no = fs_find_sub_fs(parent_no, fs_name);
			if (child_no == 0)
			{
				free_mm(fs_name, FS_NAME_LENGTH);
				return NULL;
			}
			parent_no = child_no;
		}
		if (file_name[i + 1] == '\0')
		{
			fs_name[j++] = '\0';
			//查找文件
			child_no = fs_find_sub_fs(parent_no, fs_name);
			//没找到
			if (child_no == 0)
			{
				free_mm(fs_name, FS_NAME_LENGTH);
				//返回空
				return NULL;
			}
		}
	}

	//申请文件内存
	s_file *fp = alloc_mm(sizeof(s_file));
	//读入文件内容
	read_block(hd_dev_id, child_no, &fp->fs);
	fp->fs_mode = fs_mode;
	fp->offset = 0;
	fp->offset_buff = 0;
	fp->offset_read_buff = FS_READ_BUFF;
	fp->block_offset = 0;
	fp->addr_index = 0;
	fp->first_index = 0;
	fp->sec_index = 0;
	fp->third_index = 0;
	if (fs_mode == FS_MODE_WRITE)
	{
		fp->fs.size = 0;
		fp->buff = alloc_mm(FS_DATA_COUNT);
	}
	else if (fs_mode == FS_MODE_READ)
	{
		fp->buff = alloc_mm(FS_READ_BUFF);
	}
	fs_empty_data(fp->buff);
	fp->next = NULL;

	free_mm(fs_name, FS_NAME_LENGTH);
	//返回文件指针
	return fp;
}

/*
 * f_write_block : 按块写入数据到文件
 *  - s_file *fp : 文件指针
 * return : int写入字节数
 */
int f_write_block(s_file *fp)
{
	//文件指针为空，出错
	if (fp == NULL)
	{
		return -1;
	}

	//读模式不能写文件，出错
	if (fp->fs_mode != FS_MODE_WRITE)
	{
		return -1;
	}

	//计算地址列表的索引
	fp->addr_index = fp->offset / FS_DATA_COUNT;
	//计算一级地址列表的索引
	fp->first_index = fp->addr_index / (FS_ADDR_COUNT * FS_ADDR_COUNT);
	//计算二级地址列表的索引
	fp->sec_index = (fp->addr_index % (FS_ADDR_COUNT * FS_ADDR_COUNT)) / FS_ADDR_COUNT;
	//计算三级地址列表的索引
	fp->third_index = (fp->addr_index % (FS_ADDR_COUNT * FS_ADDR_COUNT)) % FS_ADDR_COUNT;

	//一级地址
	s_fs *fst_addrs = alloc_mm(sizeof(s_fs));
	if (fp->fs.address == 0)
	{
		//创建一级磁盘指针
		fp->fs.address = alloc_sec(hd_dev_id);
		fs_empty_data(fst_addrs);
		//写入一级指针块数据
		write_block(hd_dev_id, fp->fs.address, fst_addrs);
		//写入文件根块数据
		write_block(hd_dev_id, fp->fs.dot, &fp->fs);
	}
	read_block(hd_dev_id, fp->fs.address, fst_addrs);

	//二级地址
	s_fs *sec_addrs = alloc_mm(sizeof(s_fs));
	if (fst_addrs->addr_no[fp->first_index] == 0)
	{
		//创建二级磁盘指针
		fst_addrs->addr_no[fp->first_index] = alloc_sec(hd_dev_id);
		fs_empty_data(sec_addrs);
		//写入二级指针块数据
		write_block(hd_dev_id, fst_addrs->addr_no[fp->first_index], sec_addrs);
		//写入一级指针块数据
		write_block(hd_dev_id, fp->fs.address, fst_addrs);
	}
	read_block(hd_dev_id, fst_addrs->addr_no[fp->first_index], sec_addrs);

	//三级地址
	s_fs *thrid_addrs = alloc_mm(sizeof(s_fs));
	if (sec_addrs->addr_no[fp->sec_index] == 0)
	{
		//创建三级磁盘指针
		sec_addrs->addr_no[fp->sec_index] = alloc_sec(hd_dev_id);
		fs_empty_data(thrid_addrs);
		//写入三级指针块数据
		write_block(hd_dev_id, sec_addrs->addr_no[fp->sec_index], thrid_addrs);
		//写入二级指针块数据
		write_block(hd_dev_id, fst_addrs->addr_no[fp->first_index], sec_addrs);
	}
	read_block(hd_dev_id, sec_addrs->addr_no[fp->sec_index], thrid_addrs);

	if (thrid_addrs->addr_no[fp->third_index] == 0)
	{
		//创建磁盘文件数据块
		thrid_addrs->addr_no[fp->third_index] = alloc_sec(hd_dev_id);
		//写入三级指针块数据
		write_block(hd_dev_id, sec_addrs->addr_no[fp->sec_index], thrid_addrs);
	}
	//写入文件块数据
	write_block(hd_dev_id, thrid_addrs->addr_no[fp->third_index], fp->buff);

	free_mm(thrid_addrs, sizeof(s_fs));
	free_mm(sec_addrs, sizeof(s_fs));
	free_mm(fst_addrs, sizeof(s_fs));
	//返回写入字节数
	return FS_DATA_COUNT;
}

/*
 * f_write : 写入数据到文件
 *  - s_file *fp : 文件指针
 *  - int size : 写入字节数
 *  - char *data : 写入数据
 * return : int文件当前偏移
 */
int f_write(s_file *fp, int size, char *data)
{
	//文件为空，出错
	if (fp == NULL)
	{
		return -1;
	}

	//读模式不能写入文件
	if (fp->fs_mode != FS_MODE_WRITE)
	{
		return -1;
	}

	//循环写文件
	for (int i = 0; i < size; ++i)
	{
		//拷贝数据到缓冲区
		fp->buff[fp->block_offset] = data[i];
		fp->block_offset++;
		fp->fs.size++;

		//buff写满0x200之后写入磁盘块
		if (fp->block_offset >= FS_DATA_COUNT)
		{
			//一次写入0x200字节
			f_write_block(fp);

			fp->block_offset = 0;
			fp->offset += FS_DATA_COUNT;
		}
	}
	//返回文件当前偏移
	return fp->offset;
}

/*
 * f_read_block : 按块读入文件
 *  - s_file *fp : 文件指针
 * return : int 读入字节数
 */
int f_read_block(s_file *fp, int block_ind)
{
	//文件为空，出错
	if (fp == NULL)
	{
		return -1;
	}

	//不是读模式不能读文件
	if (fp->fs_mode != FS_MODE_READ)
	{
		return -1;
	}

	//计算地址列表的索引
	fp->addr_index = fp->offset_buff / FS_DATA_COUNT;
	//计算一级地址列表的索引
	fp->first_index = fp->addr_index / (FS_ADDR_COUNT * FS_ADDR_COUNT);
	//计算二级地址列表的索引
	fp->sec_index = (fp->addr_index % (FS_ADDR_COUNT * FS_ADDR_COUNT)) / FS_ADDR_COUNT;
	//计算三级地址列表的索引
	fp->third_index = (fp->addr_index % (FS_ADDR_COUNT * FS_ADDR_COUNT)) % FS_ADDR_COUNT;

	//一级地址为空，返回
	s_fs *fst_addrs = alloc_mm(sizeof(s_fs));
	if (fp->fs.address == 0)
	{
		free_mm(fst_addrs, sizeof(s_fs));
		return 0;
	}

	//二级地址为空，返回
	read_block(hd_dev_id, fp->fs.address, fst_addrs);
	s_fs *sec_addrs = alloc_mm(sizeof(s_fs));
	if (fst_addrs->addr_no[fp->first_index] == 0)
	{
		free_mm(fst_addrs, sizeof(s_fs));
		free_mm(sec_addrs, sizeof(s_fs));
		return 0;
	}

	//三级地址为空，返回
	read_block(hd_dev_id, fst_addrs->addr_no[fp->first_index], sec_addrs);
	s_fs *thrid_addrs = alloc_mm(sizeof(s_fs));
	if (sec_addrs->addr_no[fp->sec_index] == 0)
	{
		free_mm(fst_addrs, sizeof(s_fs));
		free_mm(sec_addrs, sizeof(s_fs));
		free_mm(thrid_addrs, sizeof(s_fs));
		return 0;
	}

	//子文件项为空，返回
	read_block(hd_dev_id, sec_addrs->addr_no[fp->sec_index], thrid_addrs);
	if (thrid_addrs->addr_no[fp->third_index] == 0)
	{
		free_mm(fst_addrs, sizeof(s_fs));
		free_mm(sec_addrs, sizeof(s_fs));
		free_mm(thrid_addrs, sizeof(s_fs));
		return 0;
	}

	//读入文件块数据
	read_block(hd_dev_id, thrid_addrs->addr_no[fp->third_index], fp->buff + (FS_DATA_COUNT * block_ind));

	free_mm(fst_addrs, sizeof(s_fs));
	free_mm(sec_addrs, sizeof(s_fs));
	free_mm(thrid_addrs, sizeof(s_fs));

	//返回字节数
	return FS_DATA_COUNT;
}

void f_read_buff(s_file *fp)
{
	for (int i = 0; i < FS_READ_BUFF_BC; ++i)
	{
		if (f_read_block(fp, i) > 0)
		{
			fp->offset_buff += FS_DATA_COUNT;
			if (fp->offset_buff > fp->fs.size)
			{
				fp->offset_buff = fp->fs.size;
			}
		}
	}
}

/*
 * f_read : 读取文件内容
 *  - s_file *fp : 文件指针
 *  - int size : 读入字节数
 *  - char *data : 等待读入的数据地址
 * return : int读入字节数
 */
int f_read(s_file *fp, int size, char *data)
{
	//文件为空，出错
	if (fp == NULL)
	{
		return 0;
	}

	//文件不是读模式，出错
	if (fp->fs_mode != FS_MODE_READ)
	{
		return 0;
	}

	if (fp->offset + size >= fp->fs.size)
	{
		size = fp->fs.size - fp->offset;
	}
	int read_size = 0;
	int offset_read = 0;
	while (size >= FS_READ_BUFF)
	{
		if (fp->offset_read_buff >= FS_READ_BUFF)
		{
			f_read_buff(fp);
			fp->offset_read_buff = 0;
		}
		mmcopy_with(fp->buff, data + offset_read, fp->offset_read_buff, FS_READ_BUFF);
		fp->offset += FS_READ_BUFF;
		fp->offset_read_buff += FS_READ_BUFF;
		offset_read += FS_READ_BUFF;
		size -= FS_READ_BUFF;
		read_size += FS_READ_BUFF;
	}
	if (size > 0 && size < FS_READ_BUFF)
	{
		if (fp->offset_read_buff >= FS_READ_BUFF)
		{
			f_read_buff(fp);
			fp->offset_read_buff = 0;
		}
		mmcopy_with(fp->buff, data + offset_read, fp->offset_read_buff, size);
		fp->offset += size;
		fp->offset_read_buff += size;
		offset_read += size;
		read_size += size;
		size = 0;
	}
	return read_size;
}

/*
 * f_is_eof : 判断是否已经是文件结尾
 *  - s_file *fp : 文件指针
 * return : 0未到结尾，1已到结尾
 */
int f_is_eof(s_file *fp)
{
	//文件为空，出错
	if (fp == NULL)
	{
		return -1;
	}

	//文件不是读模式，出错
	if (fp->fs_mode != FS_MODE_READ)
	{
		return -1;
	}

	//如果当前文件读取偏移超出文件大小，则说明已到文件结尾
	if (fp->offset >= fp->fs.size)
	{
		return 1;
	}

	//未到结尾
	return 0;
}

/*
 * f_close : 关闭文件
 *  - s_file *fp : 文件指针
 * return : -1为失败， 0 为成功
 */
int f_close(s_file *fp)
{
	//文件指针为空，出错
	if (fp == NULL)
	{
		return -1;
	}

	//如果是写模式，要将最后一次没写入完成的内容写入文件
	if (fp->fs_mode == FS_MODE_WRITE)
	{
		//如果有块内偏移
		if (fp->block_offset > 0)
		{
			//写入剩余内容
			for (int i = fp->block_offset; i < FS_DATA_COUNT; ++i)
			{
				fp->buff[i] = 0;
			}
			f_write_block(fp);
		}
		//写入文件，主要更新文件大小属性
		write_block(hd_dev_id, fp->fs.dot, &fp->fs);
	}

	//释放文件指针内存
	if (fp->fs_mode == FS_MODE_WRITE)
	{
		free_mm(fp->buff, FS_DATA_COUNT);
	}
	else if (fp->fs_mode == FS_MODE_READ)
	{
		free_mm(fp->buff, FS_READ_BUFF);
	}

	free_mm(fp, sizeof(s_file));

	//返回成功
	return 0;
}

/*
 * f_opendir : 打开文件夹，读取其子文件（夹）到文件指针链表中
 *  - char *path_name : 完整路径的文件夹名称
 * return : s_file*子文件（夹）链表头
 */
s_file* f_opendir(char *path_name)
{
	//临时保存文件夹名称
	char *folder_name = alloc_mm(FS_NAME_LENGTH);
	//父级文件块编号
	u32 parent_no = 0;
	for (int i = 0, j = 0; path_name[i] != '\0'; ++i)
	{
		//以'/'为分割符取得文件夹名称
		if (path_name[i] != '/')
		{
			folder_name[j++] = path_name[i];
		}
		else
		{
			//取得文件夹名称
			folder_name[j++] = '/';
			folder_name[j++] = '\0';
			j = 0;
			//根据父级文件块编号查找其下的直属子文件
			int child_no = fs_find_sub_fs(parent_no, folder_name);
			//找到子文件块，将其设定为下一级父文件块编号
			parent_no = child_no;
		}
	}
	if (parent_no == 0)
	{
		free_mm(folder_name, FS_NAME_LENGTH);
		return NULL;
	}

	//读取文件块内容
	s_fs *fs = alloc_mm(sizeof(s_fs));
	read_block(hd_dev_id, parent_no, fs);

	if (fs->address == 0)
	{
		free_mm(folder_name, FS_NAME_LENGTH);
		return NULL;
	}
	free_mm(fs, sizeof(s_fs));

	s_file *files = NULL;
	s_file *p = files;

	//一级地址列表
	s_fs *fst_addrs = alloc_mm(sizeof(s_fs));
	read_block(hd_dev_id, fs->address, fst_addrs);

	for (int i = 0; i < FS_ADDR_COUNT; ++i)
	{
		if (fst_addrs->addr_no[i] != 0)
		{
			//二级地址列表
			s_fs *sec_addrs = alloc_mm(sizeof(s_fs));
			read_block(hd_dev_id, fst_addrs->addr_no[i], sec_addrs);

			for (int j = 0; j < FS_ADDR_COUNT; ++j)
			{
				if (sec_addrs->addr_no[j] != 0)
				{
					//三级地址列表
					s_fs *thrid_addrs = alloc_mm(sizeof(s_fs));
					read_block(hd_dev_id, sec_addrs->addr_no[j], thrid_addrs);
					for (int k = 0; k < FS_ADDR_COUNT; ++k)
					{
						if (thrid_addrs->addr_no[k] != 0)
						{
							//读取文件内容
							s_fs *fs_b = alloc_mm(sizeof(s_fs));
							read_block(hd_dev_id, thrid_addrs->addr_no[k], fs_b);
							if (p == NULL)
							{
								//申请内存给链表
								p = alloc_mm(sizeof(s_file));
							}
							else
							{
								//挂接链表
								p->next = alloc_mm(sizeof(s_file));
								p = p->next;
							}
							p->next = NULL;
							if (files == NULL)
							{
								files = p;
							}
							//复制文件内容
							mmcopy(fs_b, &(p->fs), sizeof(s_fs));
							free_mm(fs_b, sizeof(s_fs));
						}
					}
					free_mm(thrid_addrs, sizeof(s_fs));
				}
			}
			free_mm(sec_addrs, sizeof(s_fs));
		}
	}
	free_mm(fst_addrs, sizeof(s_fs));

	free_mm(folder_name, FS_NAME_LENGTH);

	//返回文件指针链表头
	return files;
}

/*
 * f_closedir : 关闭文件链表
 *  - s_file* fp : 文件链表头
 * return : void
 */
void f_closedir(s_file* fp)
{
	//循环链表
	while (fp != NULL)
	{
		//释放内存
		s_file *p = fp;
		fp = fp->next;
		free_mm(p, sizeof(s_file));
	}
}

