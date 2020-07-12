#pragma once

#include "arch.h"

namespace Libk {

void atomicEnable() { disableInterrupts(); }
void atomicDisable() { enableInterrupts(); }

}