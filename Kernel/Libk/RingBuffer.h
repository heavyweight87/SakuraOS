#pragma once

#include <cstddef>
#include "Scheduler.h"

namespace Libk {

template <typename Type, std::size_t maxElements>
class RingBuffer {
    public:
        RingBuffer() {}
        bool isFull() const { return m_numElements >= maxElements; } 
        bool isEmpty() const { return m_numElements == 0; } 

        bool push(const Type& element, bool force = false)
        {
            bool full = false;
            Kernel::Scheduler::disable(); 
            full = isFull();
            if (!full || force) 
            {
                m_buffer[m_head] = element;
                m_head = (m_head + 1)%maxElements;
                m_numElements = full ? m_numElements : (m_numElements + 1);
            }
            Kernel::Scheduler::enable();
            return !full;
        }
        
        bool pop(Type *element)
        {
            bool ret = false;
            size_t tail;

            Kernel::Scheduler::disable(); 
            {
                if (!isEmpty()) {
                    tail = getTail();
                    *element = m_buffer[tail];
                    m_numElements--;

                    ret = true;
                }
            }
            Kernel::Scheduler::enable(); 

            return ret;
        }

        std::size_t numElements() const { return m_numElements; } 
    private:
        std::size_t getTail() const { return (m_head + (maxElements - m_numElements))%maxElements; } 
        Type m_buffer[maxElements];
        std::size_t m_head = 0;
        std::size_t m_numElements = 0;
};

}