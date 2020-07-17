#pragma once

#include "Arch.h"

namespace Libk {

void atomicEnable() { /*disableInterrupts();*/ }
void atomicDisable() { /*enableInterrupts();*/ }

}