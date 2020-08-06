#pragma once

#include <cstdint>

#define TOTAL_MEMORY        (0xFFFFFFFFUL)
#define KERNEL_TOTAL_MEMORY (0x40000000UL)
#define KERNEL_START_ADDRESS (0x100000)

struct Registers 
{
	std::uint32_t gs, fs, es, ds;
	std::uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
	std::uint32_t int_no, err_code;
	std::uint32_t eip, cs, eflags, useresp, ss;
};

enum InterruptSource
{
    DivideZeroExeption = 0,
    OverflowException = 1,
    InvalidOpcodeException = 6,
    DoubleFault = 8,
    GeneralProtectionFault = 13,
    PageFault = 14,
    Timer = 32,
    Keyboard = 33,
    Com2 = 35,
    Com1,
    NumInterrupts = 48
};

static inline void outb(uint16_t port, uint8_t val)
{
    asm volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline void outl(uint16_t port, uint32_t val)
{
    asm volatile ("outl %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint32_t inl(uint16_t port)
{
    uint32_t ret;
    asm volatile ( "inl %1, %0"
                   : "=a"(ret)
                   : "Nd"(port) );
    return ret;
}

static inline uint8_t inb(uint16_t port)
{
    uint8_t ret;
    asm volatile ( "inb %1, %0"
                   : "=a"(ret)
                   : "Nd"(port) );
    return ret;
}

inline void enableInterrupts() { asm volatile("sti" : : ); }
inline void disableInterrupts() { asm volatile("cli" : : ); }
inline void halt() { asm volatile("hlt" : : ); }