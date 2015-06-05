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

#命令
#编译c程序
CC = gcc
#编译c++程序
CK = g++
#连接oject文件到elf可执行文件
LD = ld
#将elf可执行文件转为纯机器码的bin文件
OC = objcopy
#将elf可执行文件反汇编到S文件
OD = objdump
#制作iso光盘文件
DD = dd
ELF = readelf

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
DD_PARAMS_END		= 8192

##################################################################################################################

#					创建文件夹	光盘iso文件	ecc文件		启动程序		系统内核		shell程序
all:				$(MKDIR)	$(MKISO)	$(ECC)		$(BOOT)		$(KERNEL)	$(PSHELL)

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
		$(BUILD_PATH)/$(KERNEL)/osiso.o			\
		$(BUILD_PATH)/$(KERNEL)/printf.o		\
		$(BUILD_PATH)/$(KERNEL)/tty.o			\
		$(BUILD_PATH)/$(KERNEL)/sys_call.o		\
		$(BUILD_PATH)/$(KERNEL)/process.o		\
		$(BUILD_PATH)/$(KERNEL)/alloc.o 		\
		$(BUILD_PATH)/$(KERNEL)/mm.o			\
		$(BUILD_PATH)/$(KERNEL)/int.o			\
		$(BUILD_PATH)/$(KERNEL)/key.o			\
		$(BUILD_PATH)/$(KERNEL)/page.o			\
		$(BUILD_PATH)/$(KERNEL)/swap.o			\
		$(BUILD_PATH)/$(KERNEL)/rand.o			\
		$(BUILD_PATH)/$(KERNEL)/hd.o			\
		$(BUILD_PATH)/$(KERNEL)/sys_var.o		\
		$(BUILD_PATH)/$(KERNEL)/fs.o			\
		$(BUILD_PATH)/$(KERNEL)/string.o		\
		$(BUILD_PATH)/$(KERNEL)/mount.o			\
		$(BUILD_PATH)/$(KERNEL)/tree.o			\
		$(BUILD_PATH)/$(KERNEL)/sche.o			\
		$(BUILD_PATH)/$(KERNEL)/list.o			\
		$(BUILD_PATH)/$(KERNEL)/math.o			\
		$(BUILD_PATH)/$(KERNEL)/math_S.o		\
		$(BUILD_PATH)/$(KERNEL)/malloc.o				

#内核程序的obj文件
$(KERNEL)_objs:
	$(CC) $(GC_K_PARAMS) $(KERNEL)/$(KERNEL).c 			-o $(BUILD_PATH)/$(KERNEL)/$(KERNEL).o
	$(CC) $(GC_K_PARAMS) $(KERNEL)/sys_call.c 			-o $(BUILD_PATH)/$(KERNEL)/sys_call.o
	$(CC) $(GC_K_PARAMS) $(KERNEL)/process.c 			-o $(BUILD_PATH)/$(KERNEL)/process.o
	$(CC) $(GC_K_PARAMS) printf/printf.c 				-o $(BUILD_PATH)/$(KERNEL)/printf.o
	$(CC) $(GC_K_PARAMS) printf/tty.c 					-o $(BUILD_PATH)/$(KERNEL)/tty.o
	$(CC) $(GC_K_PARAMS) mm/alloc.c 					-o $(BUILD_PATH)/$(KERNEL)/alloc.o
	$(CC) $(GC_K_PARAMS) mm/mm.c 						-o $(BUILD_PATH)/$(KERNEL)/mm.o
	$(CC) $(GC_K_PARAMS) mm/int.S 						-o $(BUILD_PATH)/$(KERNEL)/int.o
	$(CC) $(GC_K_PARAMS) mm/key.c 						-o $(BUILD_PATH)/$(KERNEL)/key.o
	$(CC) $(GC_K_PARAMS) mm/page.c 						-o $(BUILD_PATH)/$(KERNEL)/page.o
	$(CC) $(GC_K_PARAMS) mm/swap.c 						-o $(BUILD_PATH)/$(KERNEL)/swap.o
	$(CC) $(GC_K_PARAMS) driver/hd/hd.c 				-o $(BUILD_PATH)/$(KERNEL)/hd.o
	$(CC) $(GC_K_PARAMS) driver/fs/fs.c 				-o $(BUILD_PATH)/$(KERNEL)/fs.o
	$(CC) $(GC_K_PARAMS) $(KERNEL)/sys_var.c 			-o $(BUILD_PATH)/$(KERNEL)/sys_var.o
	$(CC) $(GC_K_PARAMS) mm/string.c 					-o $(BUILD_PATH)/$(KERNEL)/string.o
	$(CC) $(GC_K_PARAMS) driver/hd/osiso.c 				-o $(BUILD_PATH)/$(KERNEL)/osiso.o
	$(CC) $(GC_K_PARAMS) driver/fs/mount.c 				-o $(BUILD_PATH)/$(KERNEL)/mount.o
	$(CC) $(GC_K_PARAMS) $(KERNEL)/tree.c 				-o $(BUILD_PATH)/$(KERNEL)/tree.o
	$(CC) $(GC_K_PARAMS) $(KERNEL)/list.c 				-o $(BUILD_PATH)/$(KERNEL)/list.o
	$(CC) $(GC_K_PARAMS) $(KERNEL)/sche.c 				-o $(BUILD_PATH)/$(KERNEL)/sche.o
	$(CC) $(GC_K_PARAMS) $(KERNEL)/rand.c 				-o $(BUILD_PATH)/$(KERNEL)/rand.o
	$(CC) $(GC_K_PARAMS) $(KERNEL)/math.c 				-o $(BUILD_PATH)/$(KERNEL)/math.o
	$(CC) $(GC_K_PARAMS) $(KERNEL)/math.S				-o $(BUILD_PATH)/$(KERNEL)/math_S.o
	$(CC) $(GC_K_PARAMS) mm/malloc.c 					-o $(BUILD_PATH)/$(KERNEL)/malloc.o
	
##################################################################################################################

#外壳程序
$(PSHELL):			$(PSHELL)_S

#外壳程序反汇编文件
$(PSHELL)_S:			$(PSHELL)_elf
	#system
	$(OD)  $(OD_PARAMS_32_ATT)   $(BUILD_PATH)/$(PSHELL)/system > $(BUILD_PATH)/$(PSHELL)/system_att.S
	$(OD)  $(OD_PARAMS_32_INTEL) $(BUILD_PATH)/$(PSHELL)/system > $(BUILD_PATH)/$(PSHELL)/system_intel.S
	$(ELF) $(BUILD_PATH)/$(PSHELL)/system -h > $(BUILD_PATH)/$(PSHELL)/system.elf
	$(ELF) $(BUILD_PATH)/$(PSHELL)/system -S >> $(BUILD_PATH)/$(PSHELL)/system.elf
	$(ELF) $(BUILD_PATH)/$(PSHELL)/system -s >> $(BUILD_PATH)/$(PSHELL)/system.elf
	$(ELF) $(BUILD_PATH)/$(PSHELL)/system -r >> $(BUILD_PATH)/$(PSHELL)/system.elf

	$(OD)  $(OD_PARAMS_32_ATT)   $(BUILD_PATH)/$(PSHELL)/shell > $(BUILD_PATH)/$(PSHELL)/shell_att.S
	$(OD)  $(OD_PARAMS_32_INTEL) $(BUILD_PATH)/$(PSHELL)/shell > $(BUILD_PATH)/$(PSHELL)/shell_intel.S
	
#外壳程序的ecc可重定向文件
$(PSHELL)_elf:		$(PSHELL)_objs
	#system
	$(LD) $(LD_PSHELL_PARAMS) -o $(BUILD_PATH)/$(PSHELL)/system -e start_main -r $(BUILD_PATH)/$(PSHELL)/start_main.o	\
		$(BUILD_PATH)/$(PSHELL)/system.o				\
		$(BUILD_PATH)/$(PSHELL)/string.o				\
		$(BUILD_PATH)/$(PSHELL)/stdio.o					\
		$(BUILD_PATH)/$(PSHELL)/pthread.o				\
		$(BUILD_PATH)/$(PSHELL)/stdlib.o				\
		$(BUILD_PATH)/$(PSHELL)/unistd.o				\
		$(BUILD_PATH)/$(PSHELL)/semaphore.o
		
	#cursor
	$(LD) $(LD_PSHELL_PARAMS) -o $(BUILD_PATH)/$(PSHELL)/cursor -e start_main -r $(BUILD_PATH)/$(PSHELL)/start_main.o	\
		$(BUILD_PATH)/$(PSHELL)/cursor.o				\
		$(BUILD_PATH)/$(PSHELL)/string.o				\
		$(BUILD_PATH)/$(PSHELL)/stdio.o					\
		$(BUILD_PATH)/$(PSHELL)/pthread.o				\
		$(BUILD_PATH)/$(PSHELL)/stdlib.o				\
		$(BUILD_PATH)/$(PSHELL)/unistd.o				\
		$(BUILD_PATH)/$(PSHELL)/semaphore.o
		
	#shell
	$(LD) $(LD_PSHELL_PARAMS) -o $(BUILD_PATH)/$(PSHELL)/shell -e start_main -r $(BUILD_PATH)/$(PSHELL)/start_main.o	\
		$(BUILD_PATH)/$(PSHELL)/shell.o					\
		$(BUILD_PATH)/$(PSHELL)/path.o					\
		$(BUILD_PATH)/$(PSHELL)/string.o				\
		$(BUILD_PATH)/$(PSHELL)/stdio.o					\
		$(BUILD_PATH)/$(PSHELL)/pthread.o				\
		$(BUILD_PATH)/$(PSHELL)/stdlib.o				\
		$(BUILD_PATH)/$(PSHELL)/unistd.o				\
		$(BUILD_PATH)/$(PSHELL)/semaphore.o				\
		$(BUILD_PATH)/$(PSHELL)/stack.o					\
		$(BUILD_PATH)/$(PSHELL)/fs.o
		
	#pwd
	$(LD) $(LD_PSHELL_PARAMS) -o $(BUILD_PATH)/$(PSHELL)/pwd -e start_main -r $(BUILD_PATH)/$(PSHELL)/start_main.o	\
		$(BUILD_PATH)/$(PSHELL)/pwd.o					\
		$(BUILD_PATH)/$(PSHELL)/stdio.o					\
		$(BUILD_PATH)/$(PSHELL)/stdlib.o				\
		$(BUILD_PATH)/$(PSHELL)/string.o
		
	#cd
	$(LD) $(LD_PSHELL_PARAMS) -o $(BUILD_PATH)/$(PSHELL)/cd -e start_main -r $(BUILD_PATH)/$(PSHELL)/start_main.o	\
		$(BUILD_PATH)/$(PSHELL)/cd.o					\
		$(BUILD_PATH)/$(PSHELL)/path.o					\
		$(BUILD_PATH)/$(PSHELL)/stdio.o					\
		$(BUILD_PATH)/$(PSHELL)/stdlib.o				\
		$(BUILD_PATH)/$(PSHELL)/stack.o					\
		$(BUILD_PATH)/$(PSHELL)/fs.o					\
		$(BUILD_PATH)/$(PSHELL)/string.o
		
	#ls
	$(LD) $(LD_PSHELL_PARAMS) -o $(BUILD_PATH)/$(PSHELL)/ls -e start_main -r $(BUILD_PATH)/$(PSHELL)/start_main.o	\
		$(BUILD_PATH)/$(PSHELL)/ls.o					\
		$(BUILD_PATH)/$(PSHELL)/path.o					\
		$(BUILD_PATH)/$(PSHELL)/stdio.o					\
		$(BUILD_PATH)/$(PSHELL)/stdlib.o				\
		$(BUILD_PATH)/$(PSHELL)/stack.o					\
		$(BUILD_PATH)/$(PSHELL)/fs.o					\
		$(BUILD_PATH)/$(PSHELL)/string.o

	#mkdir
	$(LD) $(LD_PSHELL_PARAMS) -o $(BUILD_PATH)/$(PSHELL)/mkdir -e start_main -r $(BUILD_PATH)/$(PSHELL)/start_main.o	\
		$(BUILD_PATH)/$(PSHELL)/mkdir.o					\
		$(BUILD_PATH)/$(PSHELL)/path.o					\
		$(BUILD_PATH)/$(PSHELL)/stdio.o					\
		$(BUILD_PATH)/$(PSHELL)/stdlib.o				\
		$(BUILD_PATH)/$(PSHELL)/stack.o					\
		$(BUILD_PATH)/$(PSHELL)/fs.o					\
		$(BUILD_PATH)/$(PSHELL)/string.o
		
	#rm
	$(LD) $(LD_PSHELL_PARAMS) -o $(BUILD_PATH)/$(PSHELL)/rm -e start_main -r $(BUILD_PATH)/$(PSHELL)/start_main.o	\
		$(BUILD_PATH)/$(PSHELL)/rm.o					\
		$(BUILD_PATH)/$(PSHELL)/path.o					\
		$(BUILD_PATH)/$(PSHELL)/stdio.o					\
		$(BUILD_PATH)/$(PSHELL)/stdlib.o				\
		$(BUILD_PATH)/$(PSHELL)/stack.o					\
		$(BUILD_PATH)/$(PSHELL)/fs.o					\
		$(BUILD_PATH)/$(PSHELL)/string.o
		
	#cat
	$(LD) $(LD_PSHELL_PARAMS) -o $(BUILD_PATH)/$(PSHELL)/cat -e start_main -r $(BUILD_PATH)/$(PSHELL)/start_main.o	\
		$(BUILD_PATH)/$(PSHELL)/cat.o					\
		$(BUILD_PATH)/$(PSHELL)/path.o					\
		$(BUILD_PATH)/$(PSHELL)/stdio.o					\
		$(BUILD_PATH)/$(PSHELL)/stdlib.o				\
		$(BUILD_PATH)/$(PSHELL)/stack.o					\
		$(BUILD_PATH)/$(PSHELL)/fs.o					\
		$(BUILD_PATH)/$(PSHELL)/string.o
				
	#start_pthread
	$(LD) $(LD_PSHELL_PARAMS) -o $(BUILD_PATH)/$(PSHELL)/start_pthread -e start_pthread -r $(BUILD_PATH)/$(PSHELL)/start_pthread.o
	
	#example_sleep
	$(LD) $(LD_PSHELL_PARAMS) -o $(BUILD_PATH)/$(PSHELL)/example_sleep -e start_main -r $(BUILD_PATH)/$(PSHELL)/start_main.o	\
		$(BUILD_PATH)/$(PSHELL)/example_sleep.o			\
		$(BUILD_PATH)/$(PSHELL)/string.o				\
		$(BUILD_PATH)/$(PSHELL)/stdio.o					\
		$(BUILD_PATH)/$(PSHELL)/stdlib.o				\
		$(BUILD_PATH)/$(PSHELL)/unistd.o				\
		$(BUILD_PATH)/$(PSHELL)/semaphore.o
		
	#example_sem
	$(LD) $(LD_PSHELL_PARAMS) -o $(BUILD_PATH)/$(PSHELL)/example_sem -e start_main -r $(BUILD_PATH)/$(PSHELL)/start_main.o	\
		$(BUILD_PATH)/$(PSHELL)/example_sem.o			\
		$(BUILD_PATH)/$(PSHELL)/string.o				\
		$(BUILD_PATH)/$(PSHELL)/unistd.o				\
		$(BUILD_PATH)/$(PSHELL)/stdio.o					\
		$(BUILD_PATH)/$(PSHELL)/stdlib.o				\
		$(BUILD_PATH)/$(PSHELL)/pthread.o				\
		$(BUILD_PATH)/$(PSHELL)/semaphore.o
		
	#example_stdio
	$(LD) $(LD_PSHELL_PARAMS) -o $(BUILD_PATH)/$(PSHELL)/example_stdio -e start_main -r $(BUILD_PATH)/$(PSHELL)/start_main.o	\
		$(BUILD_PATH)/$(PSHELL)/example_stdio.o			\
		$(BUILD_PATH)/$(PSHELL)/stdio.o					\
		$(BUILD_PATH)/$(PSHELL)/stdlib.o				\
		$(BUILD_PATH)/$(PSHELL)/string.o				\
		$(BUILD_PATH)/$(PSHELL)/semaphore.o
		
	#example_fpu
	$(LD) $(LD_PSHELL_PARAMS) -o $(BUILD_PATH)/$(PSHELL)/example_fpu -e start_main -r $(BUILD_PATH)/$(PSHELL)/start_main.o	\
		$(BUILD_PATH)/$(PSHELL)/example_fpu.o			\
		$(BUILD_PATH)/$(KERNEL)/math.o					\
		$(BUILD_PATH)/$(KERNEL)/math_S.o				\
		$(BUILD_PATH)/$(PSHELL)/string.o				\
		$(BUILD_PATH)/$(PSHELL)/unistd.o				\
		$(BUILD_PATH)/$(PSHELL)/semaphore.o				\
		$(BUILD_PATH)/$(PSHELL)/stdlib.o				\
		$(BUILD_PATH)/$(PSHELL)/stdio.o
		
	#example_args
	$(LD) $(LD_PSHELL_PARAMS) -o $(BUILD_PATH)/$(PSHELL)/example_args -e start_main -r $(BUILD_PATH)/$(PSHELL)/start_main.o	\
		$(BUILD_PATH)/$(PSHELL)/example_args.o			\
		$(BUILD_PATH)/$(KERNEL)/math.o					\
		$(BUILD_PATH)/$(PSHELL)/string.o				\
		$(BUILD_PATH)/$(KERNEL)/math_S.o				\
		$(BUILD_PATH)/$(PSHELL)/unistd.o				\
		$(BUILD_PATH)/$(PSHELL)/semaphore.o				\
		$(BUILD_PATH)/$(PSHELL)/stdlib.o				\
		$(BUILD_PATH)/$(PSHELL)/stdio.o
	
	#example_icecream
	$(LD) $(LD_PSHELL_PARAMS) -o $(BUILD_PATH)/$(PSHELL)/example_icecream -e start_main -r $(BUILD_PATH)/$(PSHELL)/start_main.o	\
		$(BUILD_PATH)/$(PSHELL)/example_icecream.o		\
		$(BUILD_PATH)/$(PSHELL)/unistd.o				\
		$(BUILD_PATH)/$(PSHELL)/stdio.o					\
		$(BUILD_PATH)/$(PSHELL)/string.o				\
		$(BUILD_PATH)/$(PSHELL)/stdlib.o				\
		$(BUILD_PATH)/$(PSHELL)/pthread.o				\
		$(BUILD_PATH)/$(PSHELL)/semaphore.o

#外壳程序的obj文件
$(PSHELL)_objs:
	$(CC) $(GC_S_PARAMS) $(PSHELL)/bin/start_main.c 		-o $(BUILD_PATH)/$(PSHELL)/start_main.o
	$(CC) $(GC_S_PARAMS) $(PSHELL)/bin/start_pthread.c 		-o $(BUILD_PATH)/$(PSHELL)/start_pthread.o
	$(CC) $(GC_S_PARAMS) $(PSHELL)/bin/system.c				-o $(BUILD_PATH)/$(PSHELL)/system.o	
	$(CC) $(GC_S_PARAMS) $(PSHELL)/bin/cursor.c				-o $(BUILD_PATH)/$(PSHELL)/cursor.o
	$(CC) $(GC_S_PARAMS) $(PSHELL)/bin/shell.c				-o $(BUILD_PATH)/$(PSHELL)/shell.o	
	$(CC) $(GC_S_PARAMS) $(PSHELL)/bin/pwd.c				-o $(BUILD_PATH)/$(PSHELL)/pwd.o
	$(CC) $(GC_S_PARAMS) $(PSHELL)/bin/cd.c					-o $(BUILD_PATH)/$(PSHELL)/cd.o
	$(CC) $(GC_S_PARAMS) $(PSHELL)/bin/ls.c					-o $(BUILD_PATH)/$(PSHELL)/ls.o
	$(CC) $(GC_S_PARAMS) $(PSHELL)/bin/mkdir.c				-o $(BUILD_PATH)/$(PSHELL)/mkdir.o
	$(CC) $(GC_S_PARAMS) $(PSHELL)/bin/rm.c					-o $(BUILD_PATH)/$(PSHELL)/rm.o
	$(CC) $(GC_S_PARAMS) $(PSHELL)/bin/cat.c				-o $(BUILD_PATH)/$(PSHELL)/cat.o
	$(CC) $(GC_S_PARAMS) $(PSHELL)/lib/unistd.c				-o $(BUILD_PATH)/$(PSHELL)/unistd.o
	$(CC) $(GC_S_PARAMS) $(PSHELL)/lib/semaphore.c			-o $(BUILD_PATH)/$(PSHELL)/semaphore.o
	$(CC) $(GC_S_PARAMS) $(PSHELL)/lib/pthread.c			-o $(BUILD_PATH)/$(PSHELL)/pthread.o
	$(CC) $(GC_S_PARAMS) $(PSHELL)/lib/stdio.c				-o $(BUILD_PATH)/$(PSHELL)/stdio.o
	$(CC) $(GC_S_PARAMS) $(PSHELL)/lib/stdlib.c				-o $(BUILD_PATH)/$(PSHELL)/stdlib.o
	$(CC) $(GC_S_PARAMS) $(PSHELL)/lib/fs.c					-o $(BUILD_PATH)/$(PSHELL)/fs.o
	$(CC) $(GC_S_PARAMS) $(PSHELL)/lib/path.c				-o $(BUILD_PATH)/$(PSHELL)/path.o
	$(CC) $(GC_S_PARAMS) $(PSHELL)/lib/stack.c				-o $(BUILD_PATH)/$(PSHELL)/stack.o
	$(CC) $(GC_S_PARAMS) mm/string.c						-o $(BUILD_PATH)/$(PSHELL)/string.o
	$(CC) $(GC_S_PARAMS) $(PSHELL)/bin/example_sleep.c		-o $(BUILD_PATH)/$(PSHELL)/example_sleep.o
	$(CC) $(GC_S_PARAMS) $(PSHELL)/bin/example_sem.c		-o $(BUILD_PATH)/$(PSHELL)/example_sem.o
	$(CC) $(GC_S_PARAMS) $(PSHELL)/bin/example_stdio.c		-o $(BUILD_PATH)/$(PSHELL)/example_stdio.o
	$(CC) $(GC_S_PARAMS) $(PSHELL)/bin/example_fpu.c		-o $(BUILD_PATH)/$(PSHELL)/example_fpu.o
	$(CC) $(GC_S_PARAMS) $(PSHELL)/bin/example_args.c		-o $(BUILD_PATH)/$(PSHELL)/example_args.o
	$(CC) $(GC_S_PARAMS) $(PSHELL)/bin/example_icecream.c	-o $(BUILD_PATH)/$(PSHELL)/example_icecream.o

##################################################################################################################
	
clean:
	rm -rvf $(BUILD_PATH)
	mkdir $(BUILD_PATH)
