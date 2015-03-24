#ifndef _MKISO_CPP_
#define _MKISO_CPP_

#include <stdio.h>
#include <stdlib.h>

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

typedef struct iso_struct
{
	u8 boot_record_indicator;
	char iso_9660_identifier[5];
	u8 version_of_this_descriptor;
	char boot_system_identifier[23];
	u8 pad0;
	u8 pad1;
	u8 pad2[32];
	u8 pad3[7];
	u8 first_sector0;
	u8 first_sector1;
	u8 first_sector2;
	u8 first_sector3;
	u8 pad4[1973];

} iso_struct;

typedef struct iso_boot_catalog
{
	u8 boot_indicator;
	u8 platform_id;
	u8 reserved0;
	u8 reserved1;
	u8 pad0[26];
	u8 end_55;
	u8 end_AA;

	u8 bootable;
	u8 boot_media_type;

	//if 0 use 0x07c0
	u8 load_segment_for_image0;
	u8 load_segment_for_image1;
	u8 system_type;
	u8 pad1;
	u8 sector_count0;
	u8 sector_count1;
	u32 start_address_of_the_virtual_disk;

	u8 pad2[20];

	u8 pad3[1984];
} iso_boot_catalog;

typedef iso_struct iso;
typedef iso_boot_catalog iso_boot;

void str_copy(char *to, const char* from, int len)
{
	int i;
	for (i = 0; i < len; i++)
	{
		to[i] = from[i];
	}
}

int main(int argc, char *args[])
{
	iso iso_m;

	iso_m.boot_record_indicator = 0;
	str_copy(iso_m.iso_9660_identifier, "CD001", 5);
	iso_m.version_of_this_descriptor = 1;
	str_copy(iso_m.boot_system_identifier, "EL TORITO SPECIFICATION", 23);
	iso_m.pad0 = 0;
	iso_m.pad1 = 0;
	int i;
	for (i = 0; i < 32; i++)
	{
		iso_m.pad2[i] = 0;
	}
	for (i = 0; i < 7; i++)
	{
		iso_m.pad3[i] = 0;
	}

	iso_m.first_sector0 = 0x13;
	iso_m.first_sector1 = 0x0;
	iso_m.first_sector2 = 0x0;
	iso_m.first_sector3 = 0x0;

	for (i = 0; i < 1973; i++)
	{
		iso_m.pad4[i] = 0;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	iso_boot iso_b;

	iso_b.boot_indicator = 1;
	iso_b.platform_id = 0;
	iso_b.reserved0 = 0;
	iso_b.reserved1 = 0;
	i = 0;
	for (i = 0; i < 26; i++)
	{
		iso_b.pad0[i] = 0;
	}
	iso_b.pad0[24] = 0xAA;
	iso_b.pad0[25] = 0x55;

	iso_b.end_55 = 0x55;
	iso_b.end_AA = 0xAA;

	iso_b.bootable = 0x88;
	iso_b.boot_media_type = 0;

	iso_b.load_segment_for_image0 = 0x00;
	iso_b.load_segment_for_image1 = 0x00;
	iso_b.system_type = 0;
	iso_b.pad1 = 0;
	iso_b.sector_count0 = 0x04;
	iso_b.sector_count1 = 0x02;
	iso_b.start_address_of_the_virtual_disk = 0x00000125;

	for (i = 0; i < 20; i++)
	{
		iso_b.pad2[i] = 0;
	}

	for (i = 0; i < 1984; i++)
	{
		iso_b.pad3[i] = 0;
	}

	FILE *p = fopen(args[1], "+wb");
	fwrite((char*) &iso_m, sizeof(char), sizeof(iso_m), p);
	fclose(p);

	FILE *p2 = fopen(args[2], "+wb");
	fwrite((char*) &iso_b, sizeof(char), sizeof(iso_b), p2);
	fclose(p2);

	return 0;
}

#endif
