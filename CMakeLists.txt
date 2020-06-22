cmake_minimum_required(VERSION 3.15)
project(SakuraOS)

# configure
enable_language(ASM)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin/isodir/boot)

# set properties and variables
set(CMAKE_SYSTEM_PROCESSOR i386)
set(COMPILER_ARCH i686)
set(ARCH_FOLDER ${CMAKE_CURRENT_SOURCE_DIR}/kernel/arch/${CMAKE_SYSTEM_PROCESSOR})
set(CROSS_FOLDER /home/gcb/opt/cross/bin/)

# set language variables and flags
set(CMAKE_C_COMPILER ${CROSS_FOLDER}/${COMPILER_ARCH}-elf-gcc)
set(CMAKE_CXX_COMPILER ${CROSS_FOLDER}/${COMPILER_ARCH}-elf-g++)
set(CMAKE_ASM_COMPILER ${CROSS_FOLDER}/${COMPILER_ARCH}-elf-as)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_FLAGS "-ffreestanding -O0 -Wall -Wextra -fno-exceptions -fno-rtti")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_C_FLAGS "-ffreestanding -O0 -Wall -Wextra")
set(CMAKE_C_FLAGS_DEBUG "-g -O0")

# set the linker script
set(LINKER_SCRIPT ${ARCH_FOLDER}/linker.ld)

# get the files to compile
file(GLOB_RECURSE KERNEL_SRC kernel/*.c kernel/*.cpp)
file(GLOB_RECURSE ARCH_SRC ${ARCH_FOLDER}/*.cpp ${ARCH_FOLDER}/*.S)
file(GLOB_RECURSE LIBC_SRC libc/*.c)

# add include directories
include_directories(libc/include)
include_directories(kernel/include)

# define the binary to build with our source files
add_executable(kernel.bin ${KERNEL_SRC} ${ARCH_SRC} ${LIBC_SRC})

add_custom_command(
    TARGET kernel.bin PRE_BUILD
    COMMAND mkdir -p "bin/isodir/boot"
)

# set target props
set_target_properties(kernel.bin PROPERTIES LINK_FLAGS "-T ${LINKER_SCRIPT} -nostdlib")              

# run the script to build an iso
add_custom_command(TARGET kernel.bin POST_BUILD COMMAND /bin/sh ${CMAKE_CURRENT_SOURCE_DIR}/scripts/iso.sh)

# for qemu
add_custom_target(qemu COMMAND qemu-system-i386 -cdrom bin/kernel.iso -serial file:serial.log -monitor stdio -d cpu_reset -m 256M -no-reboot)
add_custom_target(qemu-debug COMMAND qemu-system-i386 -cdrom bin/kernel.iso -serial file:serial.log -monitor stdio -m 256M -S -gdb tcp::1234)