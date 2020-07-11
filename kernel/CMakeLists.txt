cmake_minimum_required(VERSION 3.16)
project(Kernel)

enable_language(ASM)

# configure
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/isodir/boot)
set(ARCH_FOLDER ${CMAKE_CURRENT_SOURCE_DIR}/arch/${CMAKE_SYSTEM_PROCESSOR})

include_directories(arch/i386)
include_directories(include)
include_directories(libk)

# set the linker script
set(LINKER_SCRIPT ${ARCH_FOLDER}/linker.ld)
file(GLOB_RECURSE KERNEL_SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.c ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE ARCH_SRC ${ARCH_FOLDER}/*.cpp ${ARCH_FOLDER}/*.S)

add_executable(kernel.bin ${KERNEL_SRC} ${ARCH_SRC} ${LIBK_SRC}) 

target_link_libraries(kernel.bin gcc)

# set target props
set_target_properties(kernel.bin PROPERTIES LINK_FLAGS "-T ${LINKER_SCRIPT} -nostdlib")      

add_custom_command(TARGET kernel.bin PRE_BUILD COMMAND mkdir -p "${CMAKE_BINARY_DIR}/bin/isodir/boot")    

