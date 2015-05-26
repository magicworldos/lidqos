all:
	mkdir -p build
	gcc -I../../lidqos/include/ -fno-builtin -std=c99 -m32 filesys.c \
		../../lidqos/driver/fs/fs_for_install.c		\
		../../lidqos/driver/hd/hd_for_install.c		\
		../../lidqos/mm/string.c			\
	-o build/filesys
	cp -rvf ../../lidqos/fs/ ../../lidqos/build/	
	cp ../../lidqos/build/shell/system ../../lidqos/build/fs/usr/bin/
	cp ../../lidqos/build/shell/start_pthread ../../lidqos/build/fs/usr/bin/
	cp ../../lidqos/build/shell/example_sleep ../../lidqos/build/fs/usr/bin/
	cp ../../lidqos/build/shell/example_sem ../../lidqos/build/fs/usr/bin/
	cp ../../lidqos/build/shell/example_stdio ../../lidqos/build/fs/usr/bin/
	./build/filesys

clean:
	rm -f build/filesys
