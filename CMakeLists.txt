cmake_minimum_required(VERSION 3.16)
include(i386.cmake)

project(SakuraOS)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_FLAGS "-ffreestanding -O0 -Wall -Wextra -fno-exceptions -fno-rtti")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_C_FLAGS "-ffreestanding -O0 -Wall -Wextra")
set(CMAKE_C_FLAGS_DEBUG "-g -O0")

add_subdirectory(Kernel)

add_custom_target(iso ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/bin/isodir/boot/kernel.bin)

# run the script to build an iso
add_custom_command(TARGET iso POST_BUILD
        COMMAND /bin/sh ${CMAKE_CURRENT_SOURCE_DIR}/scripts/iso.sh
        #COMMAND /bin/sh ${CMAKE_CURRENT_SOURCE_DIR}/modules/build.sh
        COMMAND objcopy --only-keep-debug bin/isodir/boot/kernel.bin kernel.sym # create object file for gdb
        DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/bin/isodir/boot/kernel.bin
        COMMENT "Building iso image")

# for qemu
add_custom_target(qemu COMMAND qemu-system-i386 -cdrom bin/kernel.iso -serial mon:stdio  -d cpu_reset -m 256M -no-reboot
-drive file=${CMAKE_SOURCE_DIR}/image.img,format=raw,index=0,media=disk -boot d DEPENDS iso)

add_custom_target(qemu-debug COMMAND qemu-system-i386 -cdrom bin/kernel.iso -serial file:serial.log -monitor stdio -m 256M -S -gdb tcp::1234
-drive file=${CMAKE_SOURCE_DIR}/image.img,format=raw,index=0,media=disk -boot d DEPENDS iso)

