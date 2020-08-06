#include "Libk.h"
#include "TTY.h"
#include "Arch.h"
#include "vga.h"

#define VGA_BUFFER_ADDRESS 0xB8000
#define VGA_ADDRESS_REGISTER 0x3D4
#define VGA_DATA_REGISTER 0x3d5

#define VGA_REG_CURSOR_START    0x0A
#define VGA_REG_CURSOR_END      0x0B
#define VGA_REG_CURSOR_LOC_HIGH 0x0E
#define VGA_REG_CURSOR_LOC_LOW  0x0F

static uint16_t* const VGA_MEMORY = (uint16_t*) VGA_BUFFER_ADDRESS;


namespace Devices {

void TTY::tty_process(void *arg)
{
    TTY *tty = (TTY*)arg;
    char c;
    while(1)
    {
        tty->m_keyboard.read(&c, 1);
        tty->writeChar(c);
        tty->updateCursor(tty->m_column, tty->m_row);
    }
}

bool TTY::open()
{
    m_row = 0;
	m_column = 0;
	m_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
	m_buffer = VGA_MEMORY;	
	clear();
	enableCursor(0, 0);
    updateCursor(0,0);
    m_task = Kernel::Scheduler::createTask(false, this);
    Kernel::Scheduler::taskStart(*m_task, tty_process);
    m_keyboard.open();
    return true;
}

void TTY::vgaWrite(int address, int value)
{
    outb(VGA_ADDRESS_REGISTER, address);
    outb(VGA_DATA_REGISTER, value);
}

int TTY::vgaRead(int address)
{
    outb(VGA_ADDRESS_REGISTER, address);
    return inb(VGA_DATA_REGISTER);
}

void TTY::updateCursor(int x, int y)
{
	int pos = getBufferIndex(x,y);
    vgaWrite(VGA_REG_CURSOR_LOC_LOW, (uint8_t) (pos & 0xFF));
	vgaWrite(VGA_REG_CURSOR_LOC_HIGH,  (uint8_t) ((pos >> 8) & 0xFF));
}

void TTY::enableCursor(uint8_t cursor_start, uint8_t cursor_end)
{
    //writting 0 to the bit 5 enables the cursor
	vgaWrite(VGA_REG_CURSOR_START, (vgaRead(VGA_REG_CURSOR_START) & 0xC0) | cursor_start);
    vgaWrite(VGA_REG_CURSOR_START, (vgaRead(VGA_REG_CURSOR_END) & 0xE0) | cursor_end);
}

void TTY::disableCursor()
{
	vgaWrite(VGA_REG_CURSOR_START, 0x20); //writting 1 to bit 5 disables the cursor
}

void TTY::clear()
{
	for (size_t y = 0; y < VGA_HEIGHT; y++) 
	{
		for (size_t x = 0; x < VGA_WIDTH; x++) 
		{
			const size_t index = y * VGA_WIDTH + x;
			m_buffer[index] = vga_entry(' ', m_color);
		}
	}
}

std::size_t TTY::read(void*buffer, std::size_t length) 
{
    (void)length;
    (void)buffer;
    return -1;
}

std::size_t TTY::write(void *buffer, std::size_t length)
{
    uint8_t *writeBuffer = (uint8_t*)buffer;
    for (size_t i = 0; i < length; i++)
    {
		writeChar(writeBuffer[i]);
    }
	updateCursor(m_column, m_row);
    return length;
}

void TTY::writeChar(char c, uint8_t color, int x, int y) 
{
	size_t index = getBufferIndex(x,y);
	if(index > (VGA_WIDTH * VGA_HEIGHT)) //go to new line
	{
		index = 0;
		clear();
		m_row = 0;
		m_column = 0;
	}
	m_buffer[index] = vga_entry(c, color);
}

void TTY::writeChar(char c) 
{
	unsigned char uc = c;
	switch(c)
	{
		case '\r':
		case '\n':
			m_row++;
			m_column = 0;
			break;
        case 0x08:
            if(m_column > 0)
            {
                m_column--;
                writeChar(' ', m_color, m_column, m_row); 
            }
            break;
		default: 	
			writeChar(uc, m_color, m_column, m_row); 
			if (++m_column == VGA_WIDTH) 
			{
				m_column = 0;
				if (++m_row == VGA_HEIGHT)
                {
					m_row = 0;
                }
			}
			break;
	}
}

}
