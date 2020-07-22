#include "InterruptHandler.h"
#include "Arch.h"
#include "Libk.h"

namespace InterruptHandler {

InterruptCallback callbackTable[InterruptSource::NumInterrupts];

void interruptHandler(InterruptSource intNum)
{
    if(intNum < InterruptSource::NumInterrupts && callbackTable[intNum] != nullptr)
    {
        callbackTable[intNum]();
    }
}

void registerInterrupt(InterruptSource intNum, InterruptCallback callback)
{
    if(intNum < InterruptSource::NumInterrupts)
    {
        callbackTable[intNum] = callback;
        return;
    }
    Libk::printk("Invalid interrupt number %d. Cannot register interrupt callback\r\n");
}

}