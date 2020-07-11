#pragma once

namespace Interrupts {

void Init();
inline void EnableInterrupts() { asm volatile("sti" : : ); }
inline void DisableInterruts() { asm volatile("cli" : : ); }

}