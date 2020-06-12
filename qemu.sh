#!/bin/sh
set -e
. ./iso.sh
    
qemu-system-i386 -cdrom myos.iso -serial file:serial.log -monitor stdio -d cpu_reset --no-reboot -m 128M 
