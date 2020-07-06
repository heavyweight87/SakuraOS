export PREFIX="$HOME/opt/cross/sakura"
export TARGET=i686-sakura
export PATH="$PREFIX/bin:$PATH"
make
objcopy program.o -O binary program.bin -S 
cp program.o ../build/bin/isodir/boot/



