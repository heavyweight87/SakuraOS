#pragma once

namespace Libk {

void AtomicEnable() { asm("sti"); }
void AtomicDisable() { asm("cli"); }

}