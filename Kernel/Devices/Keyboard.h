#pragma once

#include "RingBuffer.h"
#include <cstdint>
#include "Devices/Device.h"
#include "InterruptHandler.h"

namespace Devices {

#define KEYBOARD_RINGBUF_SIZE 1024

class Keyboard : public Device, Kernel::InterruptHandler {
    public:
        Keyboard();
        std::size_t read(std::uint8_t *buffer, std::size_t length) override;
    private:
        Libk::RingBuffer<char, KEYBOARD_RINGBUF_SIZE> m_ringbuffer;
        void IrqCallback(int intNum) override;
};


}