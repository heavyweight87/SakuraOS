#pragma once

namespace Libk {

void atomicEnable() { asm("sti"); }
void atomicDisable() { asm("cli"); }

}