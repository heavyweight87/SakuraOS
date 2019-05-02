export PREFIX="$HOME/opt/cross"
export TARGET=i686-elf
export PATH="$PREFIX/bin:$PATH"
i686-elf-as program.s -o program.o
objcopy program.o -O binary program.bin
cp program.bin ../isodir/boot/grub


