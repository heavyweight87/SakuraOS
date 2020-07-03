#!/bin/sh
set -e

mkdir -p bin/isodir/boot/grub

BUILD_MODULES=false # set to true to add test module

if [$BUILD_MODULES == true];
then
	cat > bin/isodir/boot/grub/grub.cfg << EOF
	menuentry "SekuraOs" {
		multiboot /boot/kernel.bin
		module bin/isodir/program.bin
	}
EOF
    cd modules
    . ./build.sh
    cd ..
else
	cat > bin/isodir/boot/grub/grub.cfg << EOF
	set timeout=1
	set default=0 # Set the default menu entry
	menuentry "SekuraOs" {
		multiboot /boot/kernel.bin
		module /boot/program.bin
	}
EOF
fi
grub-mkrescue -o bin/kernel.iso bin/isodir
