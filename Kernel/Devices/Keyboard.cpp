#include "Keyboard.h"
#include "InterruptHandler.h"
#include "Libk.h"

#define PS2_DATA_REG 0x60

namespace Devices {

enum Keys {
    None = 0,
    Escape = 27,
    Key_1,
    Key_2,
    Key_3,
    Key_4,
    Key_5,
    Key_6,
    Key_7,
    Key_8,
    Key_9,
    Key_0,
    Backslash,
    Equals,
    Backspace,
    Tab,
    Key_q,
    Key_w,
    Key_e,
    Key_r,
    Key_t,
    Key_z,
    Key_u,
    Key_i,
    Key_p,
    BracketLeft,
    BracketRight,
    Enter,
    Control,
};

char keymap[] = {
    0,    '\033',    '1',    '2',    '3',    '4',    '5',    '6',    '7',    '8',    '9',    '0',    '-',    '=',    0x08,
    '\t',    'q',    'w',    'e',    'r',    't',    'y',    'u',    'i',    'o',    'p',    '[',    ']',    '\n',
    0,    'a',    's',    'd',    'f',    'g',    'h',    'j',    'k',    'l',    ';',   '\'',    '`',       0,
    '\\',    'z',    'x',    'c',    'v',    'b',    'n',    'm',    ',',    '.',    '/',      0,    '*',       0,
    ' ',      0,      0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '7', '8', '9', '-', '4', '5', '6', '+', '1', '2', '3', '0', '.', 0, 0, '\\', 0, 0, 0,
};

void Keyboard::irqCallback(int intNum)
{
    (void)intNum;
    std::uint8_t keycode = inb(PS2_DATA_REG);
    if(!(keycode & 0x80))
    {
        uint8_t code = keycode&0x7F;
        char c = keymap[code];
        Libk::printk("%c", c);
        m_ringbuffer.push(c);
    }
}

std::size_t Keyboard::read(std::uint8_t *buffer, std::size_t length)
{
    return 0;
}

Keyboard::Keyboard()
{
    registerInterrupt(InterruptSource::Keyboard);
}


}