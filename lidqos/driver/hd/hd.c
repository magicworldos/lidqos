#include <kernel/hd.h>

extern s_sys_var *sys_var;

/*
 * read_block : 读块数据
 *  - u32 hd_dev_id : 块设备类型
 *  - u32 lba : 线性编号
 *  - void *data : 数据地址
 * return : void
 */
void read_block(u32 hd_dev_id, u32 lba, void *data)
{
	//系统光盘设备sys_var->hda_info->pts[0].
	if (hd_dev_id == HD_DEV_ID_OSISO)
	{
		osiso_read_block(ATA_BUS_PRIMARY, ATA_DRIVE_SLAVE, lba, (char *) data);
	}
	else if (hd_dev_id >= HD_DEV_ID_HDA_MIN && hd_dev_id <= HD_DEV_ID_HDA_MAX)
	{
		hd_rw(lba, HD_READ, data);
	}
}

/*
 * write_block : 写块数据
 *  - u32 hd_dev_id : 块设备类型
 *  - u32 lba : 线性编号
 *  - void *data : 数据地址
 * return : void
 */
void write_block(u32 hd_dev_id, u32 lba, void *data)
{
	//系统光盘设备
	if (hd_dev_id == HD_DEV_ID_OSISO)
	{
		osiso_write_block(ATA_BUS_PRIMARY, ATA_DRIVE_SLAVE, lba, (char *) data);
	}
	else if (hd_dev_id >= HD_DEV_ID_HDA_MIN && hd_dev_id <= HD_DEV_ID_HDA_MAX)
	{
		hd_rw(lba, HD_WRITE, data);
	}
}

/*
 * load_root : 载入根文件块内容
 *  - u32 hd_dev_id : 块设备类型
 *  - void *fs_root : 数据地址
 * return : void
 */
void load_root(u32 hd_dev_id, void *fs_root)
{
	//系统光盘设备
	if (hd_dev_id == HD_DEV_ID_OSISO)
	{
		osiso_load_root(fs_root);
	}
	else if (hd_dev_id >= HD_DEV_ID_HDA_MIN && hd_dev_id <= HD_DEV_ID_HDA_MAX)
	{
		for (int i = 0; i < sys_var->hda_info->pt_count; i++)
		{
			if (sys_var->hda_info->pts[i].device == hd_dev_id)
			{
				u32 lba = sys_var->hda_info->pts[i].start;
				hd_rw(lba, HD_READ, fs_root);
				break;
			}
		}
	}
}

/*
 * alloc_sec : 申请一个空的块
 *  - u32 hd_dev_id : 块设备类型
 * return : u32块编号
 */
u32 alloc_sec(u32 hd_dev_id)
{
	//系统光盘设备
	if (hd_dev_id == HD_DEV_ID_OSISO)
	{
		return osiso_alloc_sec();
	}
	else if (hd_dev_id >= HD_DEV_ID_HDA_MIN && hd_dev_id <= HD_DEV_ID_HDA_MAX)
	{
		return hda_alloc_sec(hd_dev_id);
	}

	return 0;
}

u32 hda_alloc_sec(u32 dev_id)
{
	u8 bitmap[0x200];
	for (int i = 0; i < sys_var->hda_info->pt_count; i++)
	{
		if (sys_var->hda_info->pts[i].device == dev_id)
		{
			u32 bitmap_size = sys_var->hda_info->pts[i].size / 8 / 0x200;
			for (int j = 0; j < bitmap_size / 0x200; j++)
			{
				hd_rw(sys_var->hda_info->pts[i].bitmap_start + j, HD_READ, bitmap);
				for (int k = 0; k < 0x200; k++)
				{
					for (int l = 0; l < 8; l++)
					{
						if (((bitmap[k] >> l) & 0x1) == 0)
						{
							bitmap[k] |= (0x1 << l);
							hd_rw(sys_var->hda_info->pts[i].bitmap_start + j, HD_WRITE, bitmap);
							return k * 8 + l + sys_var->hda_info->pts[i].start;
						}
					}
				}
			}
			break;
		}
	}
	return 0;
}

void free_sec(u32 hd_dev_id, u32 lba)
{
	if (hd_dev_id >= HD_DEV_ID_HDA_MIN && hd_dev_id <= HD_DEV_ID_HDA_MAX)
	{
		return hda_free_sec(hd_dev_id, lba);
	}
}

void hda_free_sec(u32 hd_dev_id, u32 lba)
{
	u8 bitmap[0x200];
	for (int i = 0; i < sys_var->hda_info->pt_count; i++)
	{
		if (sys_var->hda_info->pts[i].device == hd_dev_id)
		{
			u32 logic_lba = lba - sys_var->hda_info->pts[i].start;
			u32 ind = logic_lba / 8 / 0x200;
			hd_rw(sys_var->hda_info->pts[i].bitmap_start + ind, HD_READ, bitmap);
			bitmap[logic_lba / 8] |= (0x1 << logic_lba % 8);
			hd_rw(sys_var->hda_info->pts[i].bitmap_start + ind, HD_WRITE, bitmap);
			break;
		}
	}
}

void hd_rw_cmd(u32 lba, u8 com, u8* buff)
{
	u8 lba0 = (u8) (lba & 0xff);
	u8 lba1 = (u8) (lba >> 8 & 0xff);
	u8 lba2 = (u8) (lba >> 16 & 0xff);
	u8 lba3 = (u8) (lba >> 24 & 0xf);
	lba3 |= 0xe0; // 1110 0000
	u16 sects_to_access = 1;
	outb_p(sects_to_access, HD_PORT_SECT_COUNT);
	outb_p(lba0, HD_PORT_LBA0);
	outb_p(lba1, HD_PORT_LBA1);
	outb_p(lba2, HD_PORT_LBA2);
	outb_p(lba3, HD_PORT_LBA3);
	outb_p(com, HD_PORT_COMMAND);
	if (com == HD_WRITE)
	{
		outsl(buff, HD_PORT_DATA, sects_to_access << 7);
	}
}

void hd_rw_data(u8 com, u8* buff)
{
	u16 sects_to_access = 1;
	if (com == HD_READ)
	{
		insl(buff, HD_PORT_DATA, sects_to_access << 7);
	}
	inb_p(HD_PORT_STATUS);
}

void hd_rw_insert_queue(s_hda_info *hda_info, u32 lba, u8 com, u8 *buff, int *status)
{
	s_hda_rw *p = alloc_mm(sizeof(s_hda_rw));
	p->lba = lba;
	p->com = com;
	p->buff = buff;
	p->status = status;
	p->next = NULL;
	//空队列
	if (hda_info->hda_rw_header == NULL)
	{
		hda_info->hda_rw_header = p;
		hda_info->hda_rw_footer = p;
	}
	//加入队列尾
	else
	{
		hda_info->hda_rw_footer->next = p;
		hda_info->hda_rw_footer = p;
	}
}

void hd_rw_execute(s_hda_info *hda_info)
{
	s_hda_rw *header = hda_info->hda_rw_header;
	if (header != NULL)
	{
		//发送读写命令
		if (*(header->status) == 0)
		{
			*(header->status) = 1;
			hd_rw_cmd(header->lba, header->com, header->buff);
		}
	}
}

void hd_rw_finished(s_hda_info *hda_info)
{
	s_hda_rw *header = hda_info->hda_rw_header;
	if (header != NULL)
	{
		if (*(header->status) == 1)
		{
			*(header->status) = 2;
			hd_rw_data(header->com, header->buff);
			s_hda_rw *p = hda_info->hda_rw_header;
			hda_info->hda_rw_header = hda_info->hda_rw_header->next;
			if (hda_info->hda_rw_header == NULL)
			{
				hda_info->hda_rw_footer = NULL;
			}
			free_mm(p, sizeof(s_hda_rw));
		}
	}
}

void hd_rw(u32 lba, u8 com, void *buf)
{
	u16 sects_to_access = 1;
	u8 s = 0;
	u16 try_times = 0x10;
	do
	{
		void *buf2 = buf;
		u8 lba0 = (u8) (lba & 0xff);
		u8 lba1 = (u8) (lba >> 8 & 0xff);
		u8 lba2 = (u8) (lba >> 16 & 0xff);
		u8 lba3 = (u8) (lba >> 24 & 0xf);
		lba3 |= 0xe0; // 1110 0000

		while (inb_p(HD_PORT_STATUS) < 0)
		{
		}

		while ((inb_p(HD_PORT_STATUS) & 0xc0) != 0x40)
		{
		}

		outb_p(sects_to_access, HD_PORT_SECT_COUNT);
		outb_p(lba0, HD_PORT_LBA0);
		outb_p(lba1, HD_PORT_LBA1);
		outb_p(lba2, HD_PORT_LBA2);
		outb_p(lba3, HD_PORT_LBA3);
		outb_p(com, HD_PORT_COMMAND);

		while (inb_p(HD_PORT_STATUS) < 0)
		{
		}

		while ((inb_p(HD_PORT_STATUS) & 0xc0) != 0x40)
		{
		}

		if (com == HD_READ)
		{
			insl(buf2, HD_PORT_DATA, sects_to_access << 7);
		}
		else if (com == HD_WRITE)
		{
			outsl(buf2, HD_PORT_DATA, sects_to_access << 7);
		}
		s = inb_p(HD_PORT_STATUS);
		if (((com == HD_READ) && (s != 0x50)) || ((com == HD_WRITE) && (s != 0x90)))
		{
			//printf(0, "\n%x", s);
		}

		if (try_times-- == 0)
		{
			break;
		}
	}
	while (((com == HD_READ) && (s != 0x50)) || ((com == HD_WRITE) && (s != 0x90)));
}
