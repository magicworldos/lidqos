//============================================================================
// Name        : ecc.cpp
// Author      : lidq
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C, Ansi-style
//============================================================================

#include <iostream>
#include <fstream>
#include <string.h>

using namespace std;

typedef unsigned int u32;
typedef signed int s32;
typedef unsigned short u16;
typedef unsigned char u8;

typedef u32 __u32;
typedef s32 __s32;
typedef u16 __u16;

/* 32-bit ELF base types. */
typedef __u32 Elf32_Addr;
typedef __u16 Elf32_Half;
typedef __u32 Elf32_Off;
typedef __s32 Elf32_Sword;
typedef __u32 Elf32_Word;

#define EI_NIDENT 16

typedef struct elf32_hdr
{
		unsigned char e_ident[EI_NIDENT];
		Elf32_Half e_type;
		Elf32_Half e_machine;
		Elf32_Word e_version;
		Elf32_Addr e_entry; /* Entry point */
		Elf32_Off e_phoff;
		Elf32_Off e_shoff;
		Elf32_Word e_flags;
		Elf32_Half e_ehsize;
		Elf32_Half e_phentsize;
		Elf32_Half e_phnum;
		Elf32_Half e_shentsize;
		Elf32_Half e_shnum;
		Elf32_Half e_shstrndx;
} Elf32_Ehdr;

typedef struct elf32_phdr
{
		Elf32_Word p_type;
		Elf32_Off p_offset;
		Elf32_Addr p_vaddr;
		Elf32_Addr p_paddr;
		Elf32_Word p_filesz;
		Elf32_Word p_memsz;
		Elf32_Word p_flags;
		Elf32_Word p_align;
} Elf32_Phdr;

typedef struct elf32_shdr
{
		Elf32_Word sh_name;
		Elf32_Word sh_type;
		Elf32_Word sh_flags;
		Elf32_Addr sh_addr;
		Elf32_Off sh_offset;
		Elf32_Word sh_size;
		Elf32_Word sh_link;
		Elf32_Word sh_info;
		Elf32_Word sh_addralign;
		Elf32_Word sh_entsize;
} Elf32_Shdr;

typedef struct elf32_rel
{
		Elf32_Addr r_offset;
		Elf32_Word r_info;
} Elf32_Rel;

class Ecc
{
	public:
		Ecc(const char *_ecc_file, const char *_elf_file, const char *_output) :
				ecc_file(0), elf_file(0), output(0), elf_data(0), elf_data_size(0), output_data_ehdr(0), output_data_ehdr_size(0), output_data_phdr(0), output_data_phdr_size(0), output_data_pdt(0), output_data_pdt_size(0), output_data_shdr(0), output_data_shdr_size(0), output_data_s_symtab(0), output_data_s_symtab_size(0), output_data_s_strtab(0), output_data_s_strtab_size(0), output_data_phdr_rel(0), rel_data(0), rel_data_size(0)
		{
			ecc_file = new char[strlen(_ecc_file) + 1];
			elf_file = new char[strlen(_elf_file) + 1];
			output = new char[strlen(_output) + 1];

			strcpy(ecc_file, _ecc_file);
			strcpy(elf_file, _elf_file);
			strcpy(output, _output);

			elf_data_size = this->fileSize(elf_file);
			elf_data = new char[elf_data_size];
		}

		virtual ~Ecc()
		{
			delete elf_data;

			delete output;
			delete elf_file;
			delete ecc_file;
		}

		void output_elf_file()
		{
			this->read_elf_data();
			this->read_egg_rel_data();
			this->write_elf_data();
		}

	private:

		int fileSize(const char *filePath)
		{
			int fsize = 0;
			ifstream file(filePath, ios::binary);
			file.seekg(0, ios::end);
			fsize = file.tellg();
			file.close();

			return fsize;
		}

		void memcpy(char *to, const char *from, int offset, int size)
		{
			u32 i;
			u8 *d = (u8 *) to;
			u8 *s = (u8 *) from;
			for (i = 0; i < size; i++)
			{
				*(d + i) = *(s + offset + i);
			}
		}

		void show_Elf32_Ehdr(Elf32_Ehdr *ehdr)
		{
			cout << hex << endl;
			cout << "e_ident:\t" << ehdr->e_ident << endl;
			cout << "e_type:\t" << ehdr->e_type << endl;
			cout << "e_machine:\t" << ehdr->e_machine << endl;
			cout << "e_version:\t" << ehdr->e_version << endl;
			cout << "e_entry:\t" << ehdr->e_entry << endl;
			cout << "e_phoff:\t" << ehdr->e_phoff << endl;
			cout << "e_shoff:\t" << ehdr->e_shoff << endl;
			cout << "e_flags:\t" << ehdr->e_flags << endl;
			cout << "e_ehsize:\t" << ehdr->e_ehsize << endl;
			cout << "e_phentsize:\t" << ehdr->e_phentsize << endl;
			cout << "e_phnum:\t" << ehdr->e_phnum << endl;
			cout << "e_shentsize:\t" << ehdr->e_shentsize << endl;
			cout << "e_shnum:\t" << ehdr->e_shnum << endl;
			cout << "e_shstrndx:\t" << ehdr->e_shstrndx << endl;
			cout << dec << endl;
		}

		void show_ELF32_Pdt(char *pdt, int size)
		{
			cout << endl;
			for (int i = 0; i < size; ++i)
			{
				printf("%x ", (u8) pdt[i]);
			}
			cout << endl;
		}

		void show_Elf32_Phdr(Elf32_Phdr *ehdr)
		{
			cout << hex << endl;
			cout << "p_type:\t" << ehdr->p_type << endl;
			cout << "p_offset:\t" << ehdr->p_offset << endl;
			cout << "p_vaddr:\t" << ehdr->p_vaddr << endl;
			cout << "p_paddr:\t" << ehdr->p_paddr << endl;
			cout << "p_filesz:\t" << ehdr->p_filesz << endl;
			cout << "p_memsz:\t" << ehdr->p_memsz << endl;
			cout << "p_flags:\t" << ehdr->p_flags << endl;
			cout << "p_align:\t" << ehdr->p_align << endl;
			cout << dec << endl;
		}

		void show_ELF32_Shdr(char *shdr, int count)
		{
			Elf32_Shdr *sh = new Elf32_Shdr();
			for (int i = 0; i < count; ++i)
			{
				this->memcpy((char *) sh, shdr, i * sizeof(Elf32_Shdr), sizeof(Elf32_Shdr));
				cout << hex;
				cout << "sh_name:" << sh->sh_name << "\t";
				cout << "sh_type:" << sh->sh_type << "\t";
				cout << "sh_flags:" << sh->sh_flags << "\t";
				cout << "sh_addr:" << sh->sh_addr << "\t";
				cout << "sh_offset:" << sh->sh_offset << "\t";
				cout << "sh_size:" << sh->sh_size << "\t";
				cout << "sh_link:" << sh->sh_link << "\t";
				cout << "sh_info:" << sh->sh_info << "\t";
				cout << "sh_addralign:" << sh->sh_addralign << "\t";
				cout << "sh_entsize:" << sh->sh_entsize << "\t";
				cout << dec << endl;
			}
			delete sh;
		}

		void show_ELF32_str_tab(char *strtab, int count)
		{
			cout << endl;
			for (int i = 0; i < count; ++i)
			{
				if (strtab[i] != '\0')
				{
					cout << strtab[i];
				}
				else if (i > 0)
				{
					cout << " ";
				}
			}
			cout << endl;
		}

		void read_elf_data()
		{
			//read elf data to elf_data
			ifstream fin(elf_file, ios::binary);
			fin.read(elf_data, elf_data_size);
			fin.close();

			//copy elf header to output_data_ehdr
			output_data_ehdr_size = sizeof(Elf32_Ehdr);
			output_data_ehdr = new char[output_data_ehdr_size];
			this->memcpy((char*) output_data_ehdr, elf_data, 0, output_data_ehdr_size);
			this->show_Elf32_Ehdr((Elf32_Ehdr *) output_data_ehdr);

			//copy program header data to output_data_phdr
			output_data_phdr_size = sizeof(Elf32_Phdr);
			output_data_phdr = new char[output_data_phdr_size];
			this->memcpy((char*) output_data_phdr, elf_data, ((Elf32_Ehdr *) output_data_ehdr)->e_phoff, output_data_phdr_size);
			this->show_Elf32_Phdr((Elf32_Phdr *) output_data_phdr);

			//copy program data & code to output_data_pdt
			output_data_pdt_size = ((Elf32_Ehdr *) output_data_ehdr)->e_shoff - output_data_ehdr_size - output_data_phdr_size;
			output_data_pdt = new char[output_data_pdt_size];
			//elf_header + pheader
			this->memcpy((char*) output_data_pdt, elf_data, output_data_ehdr_size + output_data_phdr_size, output_data_pdt_size);
			//this->show_ELF32_Pdt(output_data_pdt, output_data_pdt_size);

			//copy section header to output_data_shdr
			output_data_shdr_size = sizeof(Elf32_Shdr) * ((Elf32_Ehdr *) output_data_ehdr)->e_shnum;
			output_data_shdr = new char[output_data_shdr_size];
			//elf_header + pheader
			this->memcpy(output_data_shdr, elf_data, ((Elf32_Ehdr *) output_data_ehdr)->e_shoff, output_data_shdr_size);
			this->show_ELF32_Shdr(output_data_shdr, ((Elf32_Ehdr *) output_data_ehdr)->e_shnum);

			//copy .symtab  section to output_data_s_symtab
			int symtab_index = ((Elf32_Ehdr *) output_data_ehdr)->e_shnum - 2; 		//index = 7
			int strtab_index = ((Elf32_Ehdr *) output_data_ehdr)->e_shnum - 1; 		//index = 8

			Elf32_Shdr *sh_symtab = new Elf32_Shdr();
			this->memcpy((char *) sh_symtab, output_data_shdr, sizeof(Elf32_Shdr) * symtab_index, sizeof(Elf32_Shdr));
			output_data_s_symtab_size = sh_symtab->sh_size;
			output_data_s_symtab = new char[output_data_s_symtab_size];
			this->memcpy(output_data_s_symtab, elf_data, sh_symtab->sh_offset, output_data_s_symtab_size);
			delete sh_symtab;

			Elf32_Shdr *sh_strtab = new Elf32_Shdr();
			this->memcpy((char *) sh_strtab, output_data_shdr, sizeof(Elf32_Shdr) * strtab_index, sizeof(Elf32_Shdr));
			output_data_s_strtab_size = sh_strtab->sh_size;
			output_data_s_strtab = new char[output_data_s_strtab_size];
			this->memcpy(output_data_s_strtab, elf_data, sh_strtab->sh_offset, output_data_s_strtab_size);
			delete sh_strtab;
			this->show_ELF32_str_tab(output_data_s_strtab, output_data_s_strtab_size);

		}

		void read_egg_rel_data()
		{
			//read ecc file to ecc_data
			int ecc_data_size = this->fileSize(ecc_file);
			char *ecc_data = new char[ecc_data_size];
			ifstream fin(ecc_file, ios::binary);
			fin.read(ecc_data, ecc_data_size);
			fin.close();

			//read ecc's elf header
			Elf32_Ehdr ehdr;
			this->memcpy((char *) &ehdr, ecc_data, 0, sizeof(Elf32_Ehdr));
			this->show_Elf32_Ehdr(&ehdr);

			//read section header
			Elf32_Shdr shdr[ehdr.e_shnum];
			this->memcpy((char *) shdr, ecc_data, ehdr.e_shoff, sizeof(Elf32_Shdr) * ehdr.e_shnum);

			//read rel_section
			int rel_count = 0;
			for (int i = 0; i < ehdr.e_shnum; ++i)
			{
				//this section is rel_text or rel_frame
				if (shdr[i].sh_type == 9)
				{
					this->show_ELF32_Shdr((char*) &shdr[i], 1);

					for (int j = 0; j < shdr[i].sh_size / sizeof(Elf32_Rel); ++j)
					{
						Elf32_Rel rel_r;
						this->memcpy((char *) &rel_r, ecc_data, shdr[i].sh_offset + j * sizeof(Elf32_Rel), sizeof(Elf32_Rel));
						if ((rel_r.r_info & 0xff) == 1)
						{
							rel_count++;
						}
					}

				}
			}
			int is_used_phdr_rel = 0;
			output_data_phdr_rel = new char[sizeof(Elf32_Shdr)];

			Elf32_Rel rel[rel_count];
			int rel_i = 0;
			for (int i = 0; i < ehdr.e_shnum; ++i)
			{
				//this section is rel_text or rel_frame
				if (shdr[i].sh_type == 9)
				{

					for (int j = 0; j < shdr[i].sh_size / sizeof(Elf32_Rel); ++j)
					{
						Elf32_Rel rel_r;
						this->memcpy((char *) &rel_r, ecc_data, shdr[i].sh_offset + j * sizeof(Elf32_Rel), sizeof(Elf32_Rel));
						if ((rel_r.r_info & 0xff) == 1)
						{
							if (is_used_phdr_rel == 0)
							{
								this->memcpy(output_data_phdr_rel, (char *) &shdr[i], 0, sizeof(Elf32_Shdr));
								is_used_phdr_rel++;
							}
							this->memcpy((char *) &rel[rel_i], (char *) &rel_r, 0, sizeof(Elf32_Rel));
//							cout << hex << rel[rel_i].r_info << "\t" << rel[rel_i].r_offset << endl;
							rel[rel_i].r_info &= 0xff;
							rel_i++;
						}
					}
				}
			}
			//copy rel to rel_data_size
			rel_data_size = rel_count * sizeof(Elf32_Rel);
			rel_data = new char[rel_data_size];
			this->memcpy(rel_data, (char *) &rel[0], 0, rel_data_size);
		}

		void write_elf_data()
		{
			ofstream fout(output, ios::binary);

			Elf32_Ehdr *ehdr = (Elf32_Ehdr *) output_data_ehdr;
			//insert rel section
			ehdr->e_shnum++;
			//ehdr->e_shstrndx++;
			fout.write((char *) ehdr, output_data_ehdr_size);
			fout.write(output_data_phdr, output_data_phdr_size);
			fout.write(output_data_pdt, output_data_pdt_size);

			//fout.write(output_data_shdr, output_data_shdr_size);
			int strtab_endof = 0;
			int shdrs_count = output_data_shdr_size / sizeof(Elf32_Shdr);
			Elf32_Shdr shdrs[shdrs_count];
			for (int i = 0; i < shdrs_count; ++i)
			{
				this->memcpy((char*) &shdrs[i], output_data_shdr, i * sizeof(Elf32_Shdr), sizeof(Elf32_Shdr));
				if (i >= shdrs_count - 2)
				{
					//insert rel header
					shdrs[i].sh_offset += sizeof(Elf32_Shdr);
					strtab_endof = shdrs[i].sh_offset + shdrs[i].sh_size;
				}
				fout.write((char*) &shdrs[i], sizeof(Elf32_Shdr));
			}

			Elf32_Shdr *shdr_rel = (Elf32_Shdr *) output_data_phdr_rel;
			shdr_rel->sh_name = 0;
			shdr_rel->sh_link = 0;
			shdr_rel->sh_offset = strtab_endof;
			shdr_rel->sh_size = rel_data_size;
			fout.write((char *) shdr_rel, sizeof(Elf32_Shdr));
			fout.write(output_data_s_symtab, output_data_s_symtab_size);
			fout.write(output_data_s_strtab, output_data_s_strtab_size);

			//insert ecc rel data
			fout.write(rel_data, rel_data_size);

			fout.close();
		}

	private:
		char *ecc_file;
		char *elf_file;
		char *output;

		//elf_data buffer
		char *elf_data;
		int elf_data_size;

		//output_data_00 :elf header
		char *output_data_ehdr;
		int output_data_ehdr_size;

		//output_data_01 :program header
		char *output_data_phdr;
		int output_data_phdr_size;

		//output_data_02 :program data(code & data)
		char *output_data_pdt;
		int output_data_pdt_size;

		//output_data_03 :section header data
		char *output_data_shdr;
		int output_data_shdr_size;

		//output_data_04 :symtab section data
		char *output_data_s_symtab;
		int output_data_s_symtab_size;

		//output_data_05 :strtab section data
		char *output_data_s_strtab;
		int output_data_s_strtab_size;

		//output_data_06 :rel header data
		char *output_data_phdr_rel;

		//output_data_07 :rel section data
		char *rel_data;
		int rel_data_size;
};

int main(int argc, char **args)
{
	//Ecc ecc("/work/workspace/os/lidqos/build/pshell/exam01.ecc", "/work/workspace/os/lidqos/build/pshell/exam01", "/work/workspace/os/lidqos/build/pshell/exam01_ecc");
	Ecc ecc(args[1], args[2], args[3]);
	ecc.output_elf_file();

	return 0;
}
