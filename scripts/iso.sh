#!/bin/sh
set -e

BUILD_MODULES=false # set to true to add test module

if [$BUILD_MODULES == true];
then
	cat > bin/grub.cfg << EOF
	menuentry "SekuraOs" {
		multiboot bin/kernel.binZ
		module bin/program.bin
	}
EOF
    cd modules
    . ./build.sh
    cd ..
else
	cat > bin/grub.cfg << EOF
	set timeout=1
	set default=0 # Set the default menu entry
	menuentry "SekuraOs" {
		multiboot bin/kernel.bin
	}
EOF
fi
grub-mkrescue -o bin/kernel.iso bin
