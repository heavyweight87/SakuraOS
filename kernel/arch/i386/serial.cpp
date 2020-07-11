

#include <io.h>
#include <stdbool.h>
#include <stdio.h>

namespace Serial {

#define PORT 0x3F8

#define SERIAL_OUT_BUFFER_SIZE 50
char serialPortBuffer[SERIAL_OUT_BUFFER_SIZE];

static int is_transmit_empty() 
{
   return inb(PORT + 5) & 0x20;
}
 
void Write(char a) 
{
   while (is_transmit_empty() == 0);
   outb(PORT,a);
}

int Write(const char* data, size_t length) 
{
	const unsigned char* bytes = (const unsigned char*) data;
	for (size_t i = 0; i < length; i++)
		Write(bytes[i]);
	return length;
}

int Write(const char* __restrict format, ...) 
{
/*	std::va_list va;
	va_start(va, format);
	int ret = vsnprintf(serialPortBuffer, (size_t)-1, format, va);
	if(ret > SERIAL_OUT_BUFFER_SIZE)
	{
		ret = SERIAL_OUT_BUFFER_SIZE;
	}
	va_end(va);
	return Write(serialPortBuffer, ret);*/
}


void Init()
{
    outb(PORT + 1, 0x00);    // Disable all interrupts
    outb(PORT + 3, 0x80);    // Enable DLAB (set baud rate divisor)
    outb(PORT + 0, 0x03);    // Set divisor to 3 (lo byte) 38400 baud
    outb(PORT + 1, 0x00);    //                  (hi byte)
    outb(PORT + 3, 0x03);    // 8 bits, no parity, one stop bit
    outb(PORT + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
    outb(PORT + 4, 0x0B);    // IRQs enabled, RTS/DSR set
}

}