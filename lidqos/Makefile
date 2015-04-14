#工程名称
PROJECT				= lidqos

BUILD_PATH			= build
MKDIR				= mkdir

BOOT				= boot
KERNEL				= kernel
MKISO				= mkiso
PSHELL				= pshell
TOOL				= ../tool
FS					= fs
PSHELL				= shell
ECC					= ecc

#命令
#编译c程序
CC = gcc
#连接oject文件到elf可执行文件
LD = ld
#将elf可执行文件转为纯机器码的bin文件
OC = objcopy
#将elf可执行文件反汇编到S文件
OD = objdump
#制作iso光盘文件
DD = dd

#gcc参数，编译32位代码
#-mpreferred-stack-boundary=2
GC_PARAMS			= -fno-builtin -Iinclude -c -m32 -std=c99
GC_B_PARAMS 		= $(GC_PARAMS)
GC_K_PARAMS			= $(GC_PARAMS)
GC_S_PARAMS			= $(GC_PARAMS)

#ld连接参数，
LD_BOOT_PARAMS 		= -m elf_i386 -Ttext 0x0 -Tdata 0x1000 
#ld连接内核参数
LD_KERNEL_PARAMS	= -m elf_i386 -Ttext 0x2000 -Tdata 0x0 
#ld连接shell参数
LD_PSHELL_PARAMS	= -m elf_i386 -Ttext 0x0

#转bin文件参数
OC_PARAMS			= -O binary

#转16位bin程序参数
OD_PARAMS_16		= -S -D -m i8086
#转16位at&t汇编
OD_PARAMS_16_ATT	= $(OD_PARAMS_16) -M att
#转16位intel汇编
OD_PARAMS_16_INTEL	= $(OD_PARAMS_16) -M intel

#转32位bin程序参数
OD_PARAMS_32		= -S -D -m i386
#转32位at&t汇编
OD_PARAMS_32_ATT	= $(OD_PARAMS_32) -M att
#转32位intel汇编
OD_PARAMS_32_INTEL	= $(OD_PARAMS_32) -M intel

#dd参数，光盘以0x800(2048)为一个扇区
DD_PARAMS			= bs=2048
#iso_header.bin所在扇区0x11
DD_PARAMS_HEADER	= 17
#iso_bootable.bin所在扇区0x13
DD_PARAMS_BOOTABLE	= 19
#boot.bin所在扇区0x125
DD_PARAMS_BOOT		= 293
#kernel.bin所在扇区0x129
DD_PARAMS_KERNEL	= 297
#文件系统起始扇区号0x400
DD_PARAMS_FILESYS	= 1024
#结尾内容所在扇区0x800
DD_PARAMS_END		= 4096

##################################################################################################################

#					创建文件夹	光盘iso文件		启动程序		系统内核		shell程序
all:				$(MKDIR)	$(MKISO)		$(BOOT)		$(KERNEL)	$(PSHELL)

##################################################################################################################

#创建文件夹
$(MKDIR):
	mkdir -p $(BUILD_PATH)/$(MKISO)		\
	$(BUILD_PATH)/$(BOOT) 				\
	$(BUILD_PATH)/$(KERNEL) 			\
	$(BUILD_PATH)/$(PSHELL)

##################################################################################################################

#光盘iso文件
$(MKISO):			$(MKISO)_mk $(MKISO)_S
	$(DD) $(DD_PARAMS) seek=$(DD_PARAMS_HEADER)		count=1   conv=notrunc if=$(BUILD_PATH)/$(MKISO)/iso_header.bin		of=$(BUILD_PATH)/$(MKISO)/$(PROJECT).iso
	$(DD) $(DD_PARAMS) seek=$(DD_PARAMS_BOOTABLE)	count=1   conv=notrunc if=$(BUILD_PATH)/$(MKISO)/iso_bootable.bin	of=$(BUILD_PATH)/$(MKISO)/$(PROJECT).iso
	$(DD) $(DD_PARAMS) seek=$(DD_PARAMS_END) 		count=1   conv=notrunc if=/dev/zero 								of=$(BUILD_PATH)/$(MKISO)/$(PROJECT).iso

#运行mkiso生成iso头内容
$(MKISO)_mk:		$(MKISO)_run
	$(BUILD_PATH)/$(MKISO)/$(MKISO) $(BUILD_PATH)/$(MKISO)/iso_header.bin $(BUILD_PATH)/$(MKISO)/iso_bootable.bin

#编译mkiso程序反汇编文件
$(MKISO)_S:	$(MKISO)_run
	$(OD) $(OD_PARAMS_32_ATT)   $(BUILD_PATH)/$(MKISO)/$(MKISO) > $(BUILD_PATH)/$(MKISO)/$(MKISO)_att.S
	$(OD) $(OD_PARAMS_32_INTEL) $(BUILD_PATH)/$(MKISO)/$(MKISO) > $(BUILD_PATH)/$(MKISO)/$(MKISO)_intel.S

#编译mkiso程序	
$(MKISO)_run:
	$(CC) -m32 $(TOOL)/$(MKISO)/$(MKISO).c -o $(BUILD_PATH)/$(MKISO)/$(MKISO)

##################################################################################################################

#启动程序
$(BOOT):			$(BOOT)_bin		$(BOOT)_S
	$(DD) $(DD_PARAMS) seek=$(DD_PARAMS_BOOT) count=4 conv=notrunc if=$(BUILD_PATH)/$(BOOT)/$(BOOT).bin of=$(BUILD_PATH)/$(MKISO)/$(PROJECT).iso

#启动程序的二进制文件
$(BOOT)_bin:		$(BOOT)_elfs
	$(OC) $(OC_PARAMS) $(BUILD_PATH)/$(BOOT)/$(BOOT) $(BUILD_PATH)/$(BOOT)/$(BOOT).bin

#启动程序反汇编文件
$(BOOT)_S:	$(BOOT)_elfs
	$(OD) $(OD_PARAMS_16_ATT)   $(BUILD_PATH)/$(BOOT)/$(BOOT) > $(BUILD_PATH)/$(BOOT)/$(BOOT)_att.S
	$(OD) $(OD_PARAMS_16_INTEL) $(BUILD_PATH)/$(BOOT)/$(BOOT) > $(BUILD_PATH)/$(BOOT)/$(BOOT)_intel.S

#启动程序的elf可执行文件	
$(BOOT)_elfs:	$(BOOT)_objs
	$(LD) $(LD_BOOT_PARAMS) -o $(BUILD_PATH)/$(BOOT)/$(BOOT) -e _start $(BUILD_PATH)/$(BOOT)/$(BOOT).o	\
		$(BUILD_PATH)/$(BOOT)/main.o

#启动程序的obj文件
$(BOOT)_objs:
	$(CC) $(GC_B_PARAMS) $(BOOT)/$(BOOT).S		-o $(BUILD_PATH)/$(BOOT)/$(BOOT).o
	$(CC) $(GC_B_PARAMS) $(BOOT)/main.c			-o $(BUILD_PATH)/$(BOOT)/main.o
	
##################################################################################################################

#系统内核
$(KERNEL):			$(KERNEL)_bin	$(KERNEL)_S
	$(DD) $(DD_PARAMS) seek=$(DD_PARAMS_KERNEL) count=256 conv=notrunc if=$(BUILD_PATH)/$(KERNEL)/$(KERNEL).bin of=$(BUILD_PATH)/$(MKISO)/$(PROJECT).iso

#内核程序的二进制文件
$(KERNEL)_bin:		$(KERNEL)_elfs
	$(OC) $(OC_PARAMS) $(BUILD_PATH)/$(KERNEL)/$(KERNEL) $(BUILD_PATH)/$(KERNEL)/$(KERNEL).bin

#内核程序反汇编文件
$(KERNEL)_S:		$(KERNEL)_elfs
	$(OD) $(OD_PARAMS_32_ATT)   $(BUILD_PATH)/$(KERNEL)/$(KERNEL) > $(BUILD_PATH)/$(KERNEL)/$(KERNEL)_att.S
	$(OD) $(OD_PARAMS_32_INTEL) $(BUILD_PATH)/$(KERNEL)/$(KERNEL) > $(BUILD_PATH)/$(KERNEL)/$(KERNEL)_intel.S

#内核程序的elf可执行文件
$(KERNEL)_elfs:		$(KERNEL)_objs
	$(LD) $(LD_KERNEL_PARAMS) -o $(BUILD_PATH)/$(KERNEL)/$(KERNEL) -e start_kernel $(BUILD_PATH)/$(KERNEL)/$(KERNEL).o	\
		$(BUILD_PATH)/$(KERNEL)/sys_call.o		\
		$(BUILD_PATH)/$(KERNEL)/printf.o		\
		$(BUILD_PATH)/$(KERNEL)/alloc.o 		\
		$(BUILD_PATH)/$(KERNEL)/mm.o			\
		$(BUILD_PATH)/$(KERNEL)/key.o			\
		$(BUILD_PATH)/$(KERNEL)/int.o

#内核程序的obj文件
$(KERNEL)_objs:
	$(CC) $(GC_K_PARAMS) $(KERNEL)/$(KERNEL).c 			-o $(BUILD_PATH)/$(KERNEL)/$(KERNEL).o
	$(CC) $(GC_K_PARAMS) $(KERNEL)/sys_call.c 			-o $(BUILD_PATH)/$(KERNEL)/sys_call.o
	$(CC) $(GC_K_PARAMS) $(KERNEL)/key.c 				-o $(BUILD_PATH)/$(KERNEL)/key.o
	$(CC) $(GC_K_PARAMS) printf/printf.c 				-o $(BUILD_PATH)/$(KERNEL)/printf.o
	$(CC) $(GC_K_PARAMS) mm/alloc.c 					-o $(BUILD_PATH)/$(KERNEL)/alloc.o
	$(CC) $(GC_K_PARAMS) mm/mm.c 						-o $(BUILD_PATH)/$(KERNEL)/mm.o
	$(CC) $(GC_K_PARAMS) mm/int.S 						-o $(BUILD_PATH)/$(KERNEL)/int.o
	
##################################################################################################################

#外壳程序
$(PSHELL):			$(PSHELL)_elfs	$(PSHELL)_S

#外壳程序反汇编文件
$(PSHELL)_S:			$(PSHELL)_elfs
	
#外壳程序的elf可执行文件
$(PSHELL)_elfs:		$(PSHELL)_ecc

#外壳程序的ecc可重定向文件
$(PSHELL)_ecc:		$(PSHELL)_objs
		
#外壳程序的obj文件
$(PSHELL)_objs:


clean:
	rm -rvf $(BUILD_PATH)
	mkdir $(BUILD_PATH)
