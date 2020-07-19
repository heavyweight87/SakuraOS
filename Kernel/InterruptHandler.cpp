#include "InterruptHandler.h"
#include "Arch.h"
#include "Libk.h"

namespace InterruptHandler {

InterruptCallback callbackTable[Interrupts::NumInterrupts];

void interruptHandler(int intNum)
{
    if(intNum < Interrupts::NumInterrupts && callbackTable[intNum] != nullptr)
    {
        callbackTable[intNum]();
    }
}

void registerInterrupt(int intNum, InterruptCallback callback)
{
    if(intNum < Interrupts::NumInterrupts)
    {
        callbackTable[intNum] = callback;
        return;
    }
    Libk::printk("Invalid interrupt number %d. Cannot register interrupt callback\r\n");
}

}