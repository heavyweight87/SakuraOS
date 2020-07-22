#pragma once

#include "Arch.h"

namespace InterruptHandler {

typedef void (*InterruptCallback)();

void registerInterrupt(InterruptSource intNum, InterruptCallback callback);
void interruptHandler(InterruptSource intNum);

}