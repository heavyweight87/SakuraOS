
#include "kstdio.h"
#include "tty.h"
#include <cstdarg>
#include "serial.h"
#include "printf.h"

namespace Libk {

	#define PRINTK_BUFFER_SIZE 100

char printkBuffer[PRINTK_BUFFER_SIZE];

int puts(const char* string) 
{
	Libk::printk("%s\n", string);
	return 0;
}

int putchar(int ic)
{
	char c = (char) ic;
	terminal_putchar(c);
	return ic;
}

void printk(const char* __restrict format, ...) 
{
	std::va_list va;
	va_start(va, format);
	int len = vsnprintf(printkBuffer, (size_t)-1, format, va);
	if(len > PRINTK_BUFFER_SIZE)
	{
		len = PRINTK_BUFFER_SIZE;
	}
	va_end(va);
    Serial::Write(printkBuffer, len);
    terminal_write(printkBuffer, len);
}

}