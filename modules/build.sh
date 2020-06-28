export PREFIX="$HOME/opt/cross/sakura"
export TARGET=i686-sakura
export PATH="$PREFIX/bin:$PATH"
i686-sakura-gcc program.c -o program.o
objcopy program.o -O binary program.bin
#cp program.bin ../isodir/boot/grub


