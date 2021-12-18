#pragma once

#include "Device.h"
#include "Scheduler.h"
#include "Keyboard.h"

namespace Devices {

#define VGA_WIDTH 80
#define VGA_HEIGHT 25


class TTY : public Device {
    public:
        bool open() override;
        std::size_t write(void* buffer, std::size_t length) override;
        std::size_t read(void* buffer, std::size_t length) override;
        void enableCursor(uint8_t cursor_start, uint8_t cursor_end);
        void disableCursor();
        void clear();
    private:
        void vgaWrite(int address, int value);
        void updateCursor(int x, int y);
        int vgaRead(int address);
        int getBufferIndex(int x, int y) { return (y * VGA_WIDTH) + x; }
        void scroll();
        void writeChar(char c) ;
        void writeChar(char c, uint8_t color, int x, int y);
        static void tty_process(void *arg);

        uint8_t m_color;
        int m_row;
        int m_column;
        uint16_t* m_buffer;
        Kernel::Scheduler::Task* m_task;
        Devices::Keyboard m_keyboard;


};

}

