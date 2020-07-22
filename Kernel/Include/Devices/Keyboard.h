#pragma once

#include "RingBuffer.h"
#include <cstdint>

namespace Devices {

#define KEYBOARD_RINGBUF_SIZE 1024

class Keyboard
{
    public:
        Keyboard();
    private:
        RingBufCPP<char, KEYBOARD_RINGBUF_SIZE> m_ringbuffer;
};


}