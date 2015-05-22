#define MK_ISO_FS				(1)

#include <stdio.h>
#include <stdlib.h>
#include <kernel/typedef.h>
#include <kernel/fsdef.h>
#include <kernel/tree.h>
#include <kernel/fs_for_install.h>
#include <kernel/mount.h>
#include <stdlib.h>
#include <memory.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>

#define CONFIG_INIT_OPERA			(0)

#define FS_OSISO_START_SNO			(1024)
//map大小为256KB，可表示1GB容量使用情况
#define FS_MAP_SIZE 				(256 * 1024)
//扇区大小
#define SECTOR_SIZE					(512)
//有效扇区起始于512
#define FS_SECTOR_START 			(FS_MAP_SIZE / SECTOR_SIZE)
//map占用512个扇区
#define FS_MAP_SECTOR_SIZE			FS_SECTOR_START

#define FS_MODE_WRITE				(0x0)
#define FS_MODE_READ				(0x1)

typedef struct file_s s_file;

u8 *map = NULL;

#define param(p) #p

void mmcopy(void *from, void *to, u32 n);

void mmcopy_with(void *from, void *to, int offset, int size);

void* alloc_mm(int size);

void load_map();

void format_map();

void save_map();

void str_copy(char *from, char *to);

//申请扇区
u32 osiso_alloc_sec();

//释放扇区
//void free_sec(u32 sector_no);

void osiso_load_root(s_fs *fs_root);

void osiso_read_block(u32 bus, u32 drive, u32 lba, char *data);

void osiso_write_block(u32 bus, u32 drive, u32 lba, void *buff);

void str_append(char *str, char *str2, char *result);

int str_length(char *str);

void number_to_str(char *buff, long number, int hex);

//void test_write_file();
//
//void test_read_file();

int is_folder(char* path);

int install_fs(char *path, char *os_path, u32 uid, u32 gid, u32 mode);

int install_file(char *path, char *os_path, u32 uid, u32 gid, u32 mode);

void read_tfile(char *file);

int main(int argc, char **args)
{
	load_map();

#if CONFIG_INIT_OPERA
	format_map();
	install_fs("../../lidqos/build/fs/root/", "/root/", 0, 0, 01700);
	install_fs("../../lidqos/build/fs/home/lidq/", "/home/lidq/", 1, 1, 01700);
	install_fs("../../lidqos/build/fs/", "/", 0, 0, 01755);
	save_map();
#else
//	install_file("../../lidqos/build/fs/usr/bin/shell", "/usr/bin/shell", 0, 0, 01755);
	install_file("../../lidqos/build/fs/usr/bin/system", "/usr/bin/system", 0, 0, 01755);
//	install_file("../../lidqos/build/fs/usr/bin/sys_hd", "/usr/bin/sys_hd", 0, 0, 01755);
//	install_file("../../lidqos/build/fs/usr/bin/ls", "/usr/bin/ls", 0, 0, 01755);
//	install_file("../../lidqos/build/fs/usr/bin/cd", "/usr/bin/cd", 0, 0, 01755);
//	install_file("../../lidqos/build/fs/usr/bin/pwd", "/usr/bin/pwd", 0, 0, 01755);
//	install_file("../../lidqos/build/fs/usr/bin/cat", "/usr/bin/cat", 0, 0, 01755);
//	install_file("../../lidqos/build/fs/usr/bin/mkdir", "/usr/bin/mkdir", 0, 0, 01755);
//	install_file("../../lidqos/build/fs/usr/bin/rm", "/usr/bin/rm", 0, 0, 01755);
//	install_file("../../lidqos/build/fs/usr/bin/vi", "/usr/bin/vi", 0, 0, 01755);
//	install_file("../../lidqos/build/fs/usr/bin/exam01", "/usr/bin/exam01", 0, 0, 01755);
//	install_file("../../lidqos/build/fs/usr/bin/exam02", "/usr/bin/exam02", 0, 0, 01755);
//	install_file("../../lidqos/build/fs/usr/bin/exam03", "/usr/bin/exam03", 0, 0, 01755);
//	install_file("../../lidqos/build/fs/usr/bin/exam04", "/usr/bin/exam04", 0, 0, 01755);
#endif

//	test_read_file();

//	printf("\nComplete!\n");
}

///////////////////////////////////////////////////////////////////////////////////////////////

int install_fs(char *path_src, char *os_path, u32 uid, u32 gid, u32 mode)
{
	DIR *dir;
	struct dirent *dirinfo;
	if ((dir = opendir(path_src)) == NULL)
	{
		//printf("Path is not exist \"%s\".\n", path_src);
		return -1;
	}
	while ((dirinfo = readdir(dir)) != NULL)
	{
		char old_path[0x400];
		int path_src_len = str_length(path_src);
		if (path_src[path_src_len - 1] != '/')
		{
			path_src[path_src_len++] = '/';
			path_src[path_src_len++] = '\0';
		}
		str_append(path_src, dirinfo->d_name, old_path);

		if (is_folder(old_path))
		{
			char path[0x400];
			str_append(os_path, dirinfo->d_name, path);
			int len = str_length(path);
			path[len++] = '/';
			path[len++] = '\0';

			fs_create_path(path, uid, gid, mode);

			if (strcmp(".", dirinfo->d_name) != 0 && strcmp("..", dirinfo->d_name) != 0)
			{
				install_fs(old_path, path, uid, gid, mode);
			}
		}
		else
		{
			char path[0x400];
			str_append(os_path, dirinfo->d_name, path);

			install_file(old_path, path, uid, gid, mode);
		}
	}
	closedir(dir);

	return 0;
}

int install_file(char *path, char *os_path, u32 uid, u32 gid, u32 mode)
{
	FILE *fp_src = fopen(path, "r");
	if (fp_src == NULL)
	{
		return -1;
	}

#if CONFIG_INIT_OPERA
	s_file *fp_read = f_open(os_path, FS_MODE_READ);
	if (fp_read != NULL)
	{
		//printf("HAVE %s\n", os_path);
		f_close(fp_read);
		return -1;
	}
	f_close(fp_read);
#endif

	f_create(os_path, uid, gid, mode);
	s_file *fp_dec = f_open(os_path, FS_MODE_WRITE);

	fseek(fp_src, 0L, SEEK_END);
	int size_src = ftell(fp_src);
	fseek(fp_src, 0L, SEEK_SET);
	char ch;
	for (int i = 0; i < size_src && !feof(fp_src); i++)
	{
		fread(&ch, sizeof(char), 1, fp_src);
		f_write(fp_dec, 1, &ch);
	}

	fclose(fp_src);
	f_close(fp_dec);

	return 0;
}

int is_folder(char* path)
{
	struct stat info;
	stat(path, &info);
	if (S_ISDIR(info.st_mode))
	{
		return 1;
	}
	return 0;
}

void number_to_str(char *buff, long number, int hex)
{
	char temp[200];
	char num[0x20] = "0123456789ABCDEFG";

	int i = 0;
	int length = 0;
	long rem;
	char sign = '+';

	temp[i++] = '\0';
	if (number < 0)
	{
		sign = '-';
		number = 0 - number;
	}
	else if (number == 0)
	{
		temp[i++] = '0';
	}

	while (number > 0)
	{
		rem = number % hex;
		temp[i++] = num[rem];
		number = number / hex;
	}
	if (sign == '-')
	{
		temp[i++] = sign;
	}
	length = i;

	for (i = length - 1; i >= 0; i--)
	{
		*buff++ = temp[i];
	}
}

int str_length(char *str)
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

void mmcopy(void *from, void *to, u32 n)
{
	u8 *t = (u8 *) to;
	u8 *f = (u8 *) from;
	for (u32 i = 0; i < n; i++)
	{
		*(t + i) = *(f + i);
	}
}

void mmcopy_with(void *from, void *to, int offset, int size)
{
	u32 i;
	u8 *d = (u8 *) to;
	u8 *s = (u8 *) from;
	for (i = 0; i < size; i++)
	{
		*(d + i) = *(s + offset + i);
	}
}

void* alloc_mm(int size)
{
	return malloc(size);
}

void free_mm(void *addr, int size)
{
	free(addr);
}

void load_map()
{
	map = alloc_mm(FS_MAP_SIZE);

	FILE *fp = fopen("../../lidqos/build/mkiso/lidqos.iso", "r");
	if (fp == NULL)
	{
		//printf("open iso file error.");
		return;
	}

	//start with 1024
	fseek(fp, 0x800 * FS_OSISO_START_SNO, SEEK_SET);
	fread(map, sizeof(u8), FS_MAP_SIZE, fp);

	fclose(fp);
}

void format_map()
{
	for (int i = 0; i < FS_MAP_SIZE; ++i)
	{
		//前512扇区为map占用，默认为已使用状态
		if (i < FS_MAP_SECTOR_SIZE / 8)
		{
			map[i] = 0xff;
		}
		else
		{
			map[i] = 0;
		}
	}
	u8 empty_data[0x200];
	for (int i = 0; i < 0x200; ++i)
	{
		empty_data[i] = 0x0;
	}
	//write root folder
	s_fs fs_root;
	mmcopy((char *) empty_data, (char *) &fs_root, 0x200);
	fs_root.owner = 0;
	fs_root.group = 0;
	fs_root.mode = 0755;
	fs_root.dot = osiso_alloc_sec();
	fs_root.dotdot = fs_root.dot;
	//fs_root.is_root = 1;
	str_copy("/", fs_root.name);

	osiso_write_block(0, 0, fs_root.dot, &fs_root);
}

void save_map()
{
	for (int i = 0; i < FS_MAP_SECTOR_SIZE; ++i)
	{
		osiso_write_block(0, 0, i, map + 0x200 * i);
	}
}

//申请扇区
u32 osiso_alloc_sec()
{
	for (u32 i = 0; i < FS_MAP_SIZE; ++i)
	{
		for (u32 j = 0; j < 8; ++j)
		{
			//找到一个未被使用的扇区
			if (((map[i] >> j) & 0x1) == 0)
			{
				//将此扇区标记为已使用
				map[i] |= (0x1 << j);
				return (i * 8) + j;
			}
		}
	}
	return 0;
}

////释放扇区
//void free_sec(u32 sector_no)
//{
//	//不能释放map所在的扇区
//	if (sector_no < FS_SECTOR_START)
//	{
//		return;
//	}
//
//	u32 i = sector_no / 8;
//	u32 j = sector_no % 8;
//	map[i] |= (0x1 << j);
//}

void osiso_load_root(s_fs *fs_root)
{
	osiso_read_block(0, 0, FS_SECTOR_START, (char *) fs_root);
}

void osiso_read_block(u32 bus, u32 drive, u32 lba, char *data)
{
	FILE *fp = fopen("../../lidqos/build/mkiso/lidqos.iso", "r");
	if (fp == NULL)
	{
		//printf("open iso file error.");
		return;
	}

	//start with 1024
	fseek(fp, 0x800 * FS_OSISO_START_SNO + 0x200 * lba, SEEK_SET);
	fread(data, sizeof(u8), 0x200, fp);

	fclose(fp);
}

void osiso_write_block(u32 bus, u32 drive, u32 lba, void *buff)
{
	lba = 0x800 * FS_OSISO_START_SNO / 0x200 + lba;

	u8 data[0x200];
	mmcopy(buff, data, 0x200);
	FILE *fp = fopen("build/filesys.bin", "w+");
	if (fp == NULL)
	{
		//printf("open bin file error.\n");
		return;
	}
	fwrite(data, sizeof(u8), 0x200, fp);

	fclose(fp);

	char *cmd = " /usr/bin/dd bs=512 seek=";
	char *param = " count=1 conv=notrunc if=build/filesys.bin of=../../lidqos/build/mkiso/lidqos.iso ";

	char buf[200];
	char cmd2[200];
	char cmd3[200];
	number_to_str(buf, lba, 10);
	str_append(cmd, buf, cmd2);
	str_append(cmd2, param, cmd3);

	system(cmd3);
}

//=================================================================================================

void read_tfile(char *file)
{
	s_file *fp = f_open(file, FS_MODE_READ);
	if (fp == NULL)
	{
		//printf("File \"%s\" is not exist.", file);
		return;
	}
	char ch;
	while (!f_is_eof(fp))
	{
		f_read(fp, 1, &ch);
		putchar(ch);
	}

//	int read_size = 1227;
//	printf("%d\n",fp->fs.size);
//	char buff[read_size];
//	while (!f_is_eof(fp))
//	{
//		f_read(fp, read_size, buff);
//		for (int i = 0; i < read_size; ++i)
//		{
//			putchar(buff[i]);
//		}
//	}
	f_close(fp);
}

//void test_write_file()
//{
//	int size = 0x200;
//	int data[size];
//
//	f_create("/home/lidq/Documents/exam02.ldq");
//	s_file *fp = f_open("/home/lidq/Documents/exam02.ldq", FS_MODE_WRITE);
//	for (int j = 0; j < size; ++j)
//	{
//		data[j] = j;
//	}
//	for (int i = 0; i < size / 0x200 * 4; ++i)
//	{
//		f_write(fp, 0x200, (char*) &data[i * 0x80]);
//	}
//	f_close(fp);
//
//	printf("\n");
//	for (int i = 0; i < size; ++i)
//	{
//		printf("%d ", data[i]);
//	}
//
//	printf("\n----------------\n");
//}
//
//void test_read_file()
//{
//	int size = 0x200;
//	char data[16116];
//	s_file *fp = f_open("/home/lidq/Documents/bresenham", FS_MODE_READ);
//
//	f_read(fp, fp->fs.size, data);
//	for (int i = 0; i < 16116; ++i)
//	{
//		putchar(data[i]);
//	}
//	f_close(fp);
//}

//=================================================================================================
