# set properties and variables
set(CMAKE_SYSTEM_PROCESSOR i386)
set(COMPILER_ARCH i686)
set(CROSS_FOLDER $ENV{HOME}/opt/cross/sakura/bin)

set(CMAKE_C_COMPILER_WORKS 1)
set(CMAKE_CXX_COMPILER_WORKS 1)
SET(CMAKE_SYSTEM_NAME Generic)

# set language variables and flags
set(CMAKE_C_COMPILER ${CROSS_FOLDER}/${COMPILER_ARCH}-sakura-gcc)
set(CMAKE_CXX_COMPILER ${CROSS_FOLDER}/${COMPILER_ARCH}-sakura-g++)
set(CMAKE_ASM_COMPILER ${CROSS_FOLDER}/${COMPILER_ARCH}-sakura-as)