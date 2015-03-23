BUILD = build

all:
	mkdir -p $(BUILD)
	gcc boot.S -c -m32 -std=c99 -o $(BUILD)/boot.o
	ld -m elf_i386 -Ttext 0x0 -e _start $(BUILD)/boot.o -o $(BUILD)/boot
	objcopy -O binary $(BUILD)/boot $(BUILD)/boot.bin
	dd bs=512 count=2880 if=/dev/zero of=$(BUILD)/boot.img
	dd bs=512 count=1 conv=notrunc if=$(BUILD)/boot.bin of=$(BUILD)/boot.img
	
clean:
	rm -rvf $(BUILD)