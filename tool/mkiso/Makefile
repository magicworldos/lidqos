BUILD = build

all:
	mkdir -p $(BUILD)
	gcc -m32 mkiso.c -o $(BUILD)/mkiso
	$(BUILD)/mkiso $(BUILD)/iso_header.bin $(BUILD)/iso_bootable.bin
	
clean:
	rm -rvf $(BUILD)