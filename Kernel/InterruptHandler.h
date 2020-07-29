#pragma once

#include "Arch.h"

namespace Kernel {

class InterruptHandler {
    public:
        virtual void irqCallback(int intNum) = 0;
        void registerInterrupt(InterruptSource intNum);
        static void interruptHandler(InterruptSource intNum);
    private:
        static InterruptHandler *m_callbackTable[InterruptSource::NumInterrupts];
};

}