#pragma once

#include <cstdint>

namespace Syscalls {

enum Syscall
{
    SYSCALL_OPEN,
    SYSCALL_CLOSE,
    SYSCALL_WRITE,
    SYSCALL_READ,
    SYSCALL_SBRK,
    SYSCALL_KILL,
    SYSCALL_EXIT
};

static inline int __syscall(Syscall syscall, int p1, int p2, int p3, int p4, int p5)
{
    int __ret;
    asm volatile("push %%ebx; movl %2,%%ebx; int $0x80; pop %%ebx"
                         : "=a"(__ret)
                         : "0"(syscall), "r"((int)(p1)), "c"((int)(p2)), "d"((int)(p3)), "S"((int)(p4)), "D"((int)(p5))
                         : "memory");
    return __ret;
}

std::uint32_t Handle(Syscall syscall, std::uint32_t p1, std::uint32_t p2, std::uint32_t p3, std::uint32_t p4, std::uint32_t p5);

}
