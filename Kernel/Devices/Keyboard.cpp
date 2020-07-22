#include "Keyboard.h"
#include "InterruptHandler.h"
#include "Libk.h"

#define PS2_DATA_REG 0x60

namespace Devices {

char qwertz[128] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', /* 9 */
  '9', '0', '\'', '=', '\b', /* Backspace */
  '\t',         /* Tab */
  'q', 'w', 'e', 'r',   /* 19 */
  't', 'z', 'u', 'i', 'o', 'p', '[', ']', '\n', /* Enter key */
    0,          /* 29   - Control */
  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', /* 39 */
 '\'', '`',   0,        /* Left shift */
 '\\', 'y', 'x', 'c', 'v', 'b', 'n',            /* 49 */
  'm', ',', '.', '-',   0,              /* Right shift */
  '*',
    0,  /* Alt */
  ' ',  /* Space bar */
    0,  /* Caps lock */
    0,  /* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,  /* < ... F10 */
    0,  /* 69 - Num lock*/
    0,  /* Scroll Lock */
    '7',
    '8',
    '9',
    '-',
    '4',
    '5',
    '6',
    '+',
    '1',
    '2',
    '3',
    '0',
    '.',  /* Delete Key */
    0,   0,   0,
    0,  /* F11 Key */
    0,  /* F12 Key */
    0,  /* All other keys are undefined */
};

static void keyboard_isr()
{
    std::uint8_t keycode = inb(PS2_DATA_REG);
    if(!(keycode & 0x80))
    {
        uint8_t code = keycode&0x7F;
        char c = qwertz[code];
        Libk::printk("%c", c);
    }

}

Keyboard::Keyboard()
{
    InterruptHandler::registerInterrupt(InterruptSource::Keyboard, keyboard_isr);
}


}