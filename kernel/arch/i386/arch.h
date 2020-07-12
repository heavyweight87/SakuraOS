#pragma once

#include <cstdint>

struct Registers 
{
	std::uint32_t gs, fs, es, ds;
	std::uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
	std::uint32_t int_no, err_code;
	std::uint32_t eip, cs, eflags, useresp, ss;
};

inline void enableInterrupts() { asm volatile("sti" : : ); }
inline void disableInterrupts() { asm volatile("cli" : : ); }