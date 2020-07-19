#pragma once

namespace InterruptHandler {

typedef void (*InterruptCallback)();

void registerInterrupt(int intNum, InterruptCallback callback);
void interruptHandler(int intNum);

}