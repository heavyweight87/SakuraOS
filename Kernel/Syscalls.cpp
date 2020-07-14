#include "Syscalls.h"
#include "Libk.h"

namespace Syscalls {


uint32_t SyscallOpen(char *fileName, uint32_t flags)
{
    Libk::printk("syscall open. File = %s\r\n", fileName);
    return 1;
}

uint32_t SyscallWrite(int fd, char* data, uint32_t len)
{
    if(fd == 1)
    {
        Libk::printk(data);
    }
    return 0;
}

uint32_t SyscallClose(int file)
{
    Libk::printk("syscall close. file = %d\r\n", file);
    return 0;
}

std::uint32_t Handle(Syscall syscall, std::uint32_t p1, std::uint32_t p2, std::uint32_t p3, std::uint32_t p4, std::uint32_t p5)
{
    switch(syscall)
    {
        case SYSCALL_OPEN:
            return SyscallOpen((char*)p1, p2);
            break;
        case SYSCALL_WRITE:
            return SyscallWrite(p1, (char*)p2, p3);
            break;
        case SYSCALL_CLOSE:
            return SyscallClose(p1);
        default: 
            Libk::printk("Unknown syscall %d\r\n", syscall);
            return 1;        
    }
}


}