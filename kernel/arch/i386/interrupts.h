#pragma once

namespace Interrupts {

void Init();
inline void enableInterrupts() { asm volatile("sti" : : ); }
inline void disableInterrupts() { asm volatile("cli" : : ); }

}