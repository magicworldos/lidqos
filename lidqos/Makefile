#工程名称
PROJECT				= lidqos

BUILD_PATH			= build
MKDIR				= mkdir

BOOT				= boot
KERNEL				= kernel
MKIMG				= mkimg
PSHELL				= shell

#命令
#编译c程序
CC = gcc
#连接oject文件到elf可执行文件
LD = ld
#将elf可执行文件转为纯机器码的bin文件
OC = objcopy
#将elf可执行文件反汇编到S文件
OD = objdump
#制作软盘或光盘文件
DD = dd

#gcc参数，编译32位代码
#-mpreferred-stack-boundary=2
GC_PARAMS			= -fno-builtin -Iinclude -c -m32 -std=c99
GC_B_PARAMS 		= $(GC_PARAMS)
GC_K_PARAMS			= $(GC_PARAMS)
GC_S_PARAMS			= $(GC_PARAMS)

#ld连接参数，
LD_BOOT_PARAMS 		= -m elf_i386 -Ttext 0x0
#ld连接内核参数
LD_KERNEL_PARAMS	= -m elf_i386 -Ttext 0x0
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

#dd参数一个扇区大小为512字节
DD_PARAMS			= bs=512

##################################################################################################################

#					创建文件夹	软盘img文件	启动程序		系统内核
all:				$(MKDIR)	$(MKIMG)	$(BOOT)		$(KERNEL)				

##################################################################################################################

#创建文件夹
$(MKDIR):
	mkdir -p $(BUILD_PATH)/$(MKIMG)		\
	$(BUILD_PATH)/$(BOOT) 				\
	$(BUILD_PATH)/$(KERNEL) 			\
	$(BUILD_PATH)/$(PSHELL)

##################################################################################################################

#软盘img文件
$(MKIMG):
	$(DD) $(DD_PARAMS) count=2880 if=/dev/zero of=$(BUILD_PATH)/$(MKIMG)/$(PROJECT).img

##################################################################################################################

#启动程序
$(BOOT):			$(BOOT)_bin		$(BOOT)_S
	$(DD) $(DD_PARAMS) count=16 conv=notrunc if=$(BUILD_PATH)/$(BOOT)/$(BOOT).bin of=$(BUILD_PATH)/$(MKIMG)/$(PROJECT).img

#启动程序的二进制文件
$(BOOT)_bin:		$(BOOT)_elfs
	$(OC) $(OC_PARAMS) $(BUILD_PATH)/$(BOOT)/$(BOOT) $(BUILD_PATH)/$(BOOT)/$(BOOT).bin

#启动程序反汇编文件
$(BOOT)_S:	$(BOOT)_elfs
	$(OD) $(OD_PARAMS_16_ATT)   $(BUILD_PATH)/$(BOOT)/$(BOOT) > $(BUILD_PATH)/$(BOOT)/$(BOOT)_att.S
	$(OD) $(OD_PARAMS_16_INTEL) $(BUILD_PATH)/$(BOOT)/$(BOOT) > $(BUILD_PATH)/$(BOOT)/$(BOOT)_intel.S

#启动程序的elf可执行文件	
$(BOOT)_elfs:	$(BOOT)_objs
	$(LD) $(LD_BOOT_PARAMS) -e _start $(BUILD_PATH)/$(BOOT)/$(BOOT).o -o $(BUILD_PATH)/$(BOOT)/$(BOOT)

#启动程序的obj文件
$(BOOT)_objs:
	$(CC) $(GC_B_PARAMS) $(BOOT)/$(BOOT).S -o $(BUILD_PATH)/$(BOOT)/$(BOOT).o
	
##################################################################################################################

#系统内核
$(KERNEL):			$(KERNEL)_bin	$(KERNEL)_S
	$(DD) $(DD_PARAMS) seek=16 count=1 conv=notrunc if=$(BUILD_PATH)/$(KERNEL)/$(KERNEL).bin of=$(BUILD_PATH)/$(MKIMG)/$(PROJECT).img

#内核程序的二进制文件
$(KERNEL)_bin:		$(KERNEL)_elfs
	$(OC) $(OC_PARAMS) $(BUILD_PATH)/$(KERNEL)/$(KERNEL) $(BUILD_PATH)/$(KERNEL)/$(KERNEL).bin

#内核程序反汇编文件
$(KERNEL)_S:		$(KERNEL)_elfs
	$(OD) $(OD_PARAMS_32_ATT)   $(BUILD_PATH)/$(KERNEL)/$(KERNEL) > $(BUILD_PATH)/$(KERNEL)/$(KERNEL)_att.S
	$(OD) $(OD_PARAMS_32_INTEL) $(BUILD_PATH)/$(KERNEL)/$(KERNEL) > $(BUILD_PATH)/$(KERNEL)/$(KERNEL)_intel.S


#内核程序的elf可执行文件
$(KERNEL)_elfs:		$(KERNEL)_objs
	$(LD) $(LD_KERNEL_PARAMS) -o $(BUILD_PATH)/$(KERNEL)/$(KERNEL) -e start_kernel $(BUILD_PATH)/$(KERNEL)/$(KERNEL).o

#内核程序的obj文件
$(KERNEL)_objs:
	$(CC) $(GC_K_PARAMS) $(KERNEL)/$(KERNEL).c 			-o $(BUILD_PATH)/$(KERNEL)/$(KERNEL).o

##################################################################################################################	

#外壳程序
$(PSHELL):

##################################################################################################################

clean:
	rm -rvf $(BUILD_PATH)
	mkdir $(BUILD_PATH)
