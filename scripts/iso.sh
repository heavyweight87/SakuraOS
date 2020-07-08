#!/bin/sh
set -e

mkdir -p bin/isodir/boot/grub


cat > bin/isodir/boot/grub/grub.cfg << EOF
set timeout=1
set default=0 # Set the default menu entry
menuentry "SekuraOS" {
	multiboot /boot/kernel.bin
	module /boot/program.o
}
EOF

grub-mkrescue -o bin/kernel.iso bin/isodir
