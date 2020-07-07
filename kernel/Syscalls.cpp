#include "Syscalls.h"
#include <stdio.h>

namespace Syscalls {


uint32_t SyscallOpen(char *fileName, uint32_t flags)
{
    printf("syscall open. File = %s\r\n", fileName);
    return 1;
}

uint32_t SyscallWrite(int file, char *data, uint32_t len)
{
    printf("syscall write. file = %d, data = %s, len = %d\r\n", file, data, len);
    return 0;
}

uint32_t SyscallClose(int file, char *data, uint32_t len)
{
    printf("syscall write. file = %d, data = %s, len = %d\r\n", file, data, len);
    return 0;
}

std::uint32_t Syscalls::Handle(Syscall syscall, std::uint32_t p1, std::uint32_t p2, std::uint32_t p3, std::uint32_t p4, std::uint32_t p5)
{
    switch(syscall)
    {
        case SYSCALL_OPEN:
            SyscallOpen((char*)p1, p2);
            break;
        case SYSCALL_WRITE:
            SyscallWrite(p1, (char*)p2, p3);
            break;
        case SYSCALL_CLOSE:

        
    }
      
    return 0;
}


}