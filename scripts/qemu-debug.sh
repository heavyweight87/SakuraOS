#!/bin/sh
set -e
. ./iso.sh

qemu-system-$(./target-triplet-to-arch.sh $HOST) -cdrom myos.iso -serial file:serial.log -monitor stdio -S -gdb tcp::1234
