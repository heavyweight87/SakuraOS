#pragma once

typedef enum 
{
    SYSCALL_OPEN,
    SYSCALL_CLOSE,
    SYSCALL_WRITE,
    SYSCALL_READ,
    SYSCALL_SRBK,
    SYSCALL_KILL,
    SYSCALL_EXIT
}Syscall;

static inline int syscall(Syscall syscall, int p1, int p2, int p3, int p4, int p5)
{
    int __ret;
    asm volatile("push %%ebx; movl %2,%%ebx; int $0x80; pop %%ebx"
                         : "=a"(__ret)
                         : "0"(syscall), "r"((int)(p1)), "c"((int)(p2)), "d"((int)(p3)), "S"((int)(p4)), "D"((int)(p5))
                         : "memory");
    return __ret;
}