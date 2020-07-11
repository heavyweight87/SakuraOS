#pragma once

#include "interrupts.h"

namespace Libk {

void atomicEnable() { Interrupts::disableInterrupts(); }
void atomicDisable() { Interrupts::enableInterrupts(); }

}