BUILD = build

all:
	mkdir -p $(BUILD)
	gcc boot.S -c -m32 -std=c99 -o $(BUILD)/boot.o
	ld -m elf_i386 -Ttext 0x0 -e _start $(BUILD)/boot.o -o $(BUILD)/boot
	objcopy -O binary $(BUILD)/boot $(BUILD)/boot.bin
	gcc -m32 ../tool/mkiso/mkiso.c -o $(BUILD)/mkiso
	$(BUILD)/mkiso $(BUILD)/iso_header.bin $(BUILD)/iso_bootable.bin
	dd bs=2048 seek=17		count=1 conv=notrunc if=$(BUILD)/iso_header.bin		of=$(BUILD)/boot.iso
	dd bs=2048 seek=19		count=1 conv=notrunc if=$(BUILD)/iso_bootable.bin	of=$(BUILD)/boot.iso
	dd bs=2048 seek=293 	count=1 conv=notrunc if=$(BUILD)/boot.bin 			of=$(BUILD)/boot.iso
	dd bs=2048 seek=4096	count=1 conv=notrunc if=/dev/zero					of=$(BUILD)/boot.iso
	
clean:
	rm -rvf $(BUILD)