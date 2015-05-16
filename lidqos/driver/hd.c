#include <kernel/hd.h>

/*
 * IDE磁盘读写函数
 */
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

		}

		if (try_times-- == 0)
		{
			break;
		}
	}
	while (((com == HD_READ) && (s != 0x50)) || ((com == HD_WRITE) && (s != 0x90)));
}

