

#include "Arch.h"
#include <stdbool.h>
#include <stdio.h>

namespace Serial {

#define COM1 0x3F8
#define COM2 0x2F8
#define COM3 0x3E8
#define COM4 0x2E8

#define DIVISOR_LOW(com) 	(com)
#define DIVISOR_HIGH(com) 	(com)
#define INT_REG(com) 		(com + 1)
#define INT_CTL_REG(com)	(com + 2)
#define LINE_CONTROL(com) 	(com + 3)
#define MODEM_CONTROL(com) 	(com + 4)
#define LINE_STATUS(com) 	(com + 5)

#define DLAB_BIT 0x80
#define THRE	 0x20 		//tx holding register empty (i.e. can send)

#define SERIAL_OUT_BUFFER_SIZE 50
char serialPortBuffer[SERIAL_OUT_BUFFER_SIZE];

static int canTransmit() 
{
   return inb(LINE_STATUS(COM1)) & THRE;
}
 
void Write(char a) 
{
   while (canTransmit() == 0);
   outb(COM1, a);
}

int Write(const char* data, size_t length) 
{
	const unsigned char* bytes = (const unsigned char*) data;
	for (size_t i = 0; i < length; i++)
		Write(bytes[i]);
	return length;
}


void Init()
{
	uint16_t port = COM1;
    outb(INT_REG(port), 0x00);    // Disable all interrupts
    outb(LINE_CONTROL(port), DLAB_BIT);    // Enable DLAB (set baud rate divisor)
    outb(DIVISOR_LOW(port), 0x03);    // Set divisor to 3 (lo byte) 38400 baud
    outb(DIVISOR_HIGH(port), 0x00);    //                  (hi byte)
    outb(LINE_CONTROL(port), 0x03);    // 8 bits, no parity, one stop bit
    outb(INT_CTL_REG(port), 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
    outb(MODEM_CONTROL(port), 0x0B);    // IRQs enabled, RTS/DSR set
}

}