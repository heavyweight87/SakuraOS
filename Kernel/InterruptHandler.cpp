#include "InterruptHandler.h"
#include "Arch.h"
#include "Libk.h"

namespace Kernel {

InterruptHandler *InterruptHandler::m_callbackTable[InterruptSource::NumInterrupts];


void InterruptHandler::interruptHandler(InterruptSource intNum)
{
    if(intNum < InterruptSource::NumInterrupts && m_callbackTable[intNum] != nullptr)
    {
        InterruptHandler::m_callbackTable[intNum]->IrqCallback(intNum);
    }
}

void InterruptHandler::registerInterrupt(InterruptSource intNum)
{
    if(intNum < InterruptSource::NumInterrupts)
    {
        m_callbackTable[intNum] = this;
        return;
    }
    Libk::printk("Invalid interrupt number %d. Cannot register interrupt callback\r\n");
}

}