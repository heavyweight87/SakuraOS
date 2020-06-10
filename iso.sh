#!/bin/sh
set -e
. ./build.sh

mkdir -p isodir
mkdir -p isodir/boot
mkdir -p isodir/boot/grub

cp sysroot/boot/myos.kernel isodir/boot/myos.kernel

BUILD_MODULES=false # set to true to add test module

if [$BUILD_MODULES == true];
then
	cat > isodir/boot/grub/grub.cfg << EOF
	menuentry "SekuraOs" {
		multiboot /boot/myos.kernel
		module /boot/program.bin
	}
EOF
    cd modules
    . ./build.sh
    cd ..
else
	cat > isodir/boot/grub/grub.cfg << EOF
	set timeout=1
	set default=0 # Set the default menu entry
	menuentry "SekuraOs" {
		multiboot /boot/myos.kernel
	}
EOF
fi
grub-mkrescue -o myos.iso isodir
