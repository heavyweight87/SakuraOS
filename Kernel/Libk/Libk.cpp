
#include "Libk.h"
#include "tty.h"
#include "Serial.h"

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

void* memset(void* bufptr, int value, size_t size) 
{
	unsigned char* buf = (unsigned char*) bufptr;
	for (size_t i = 0; i < size; i++)
		buf[i] = (unsigned char) value;
	return bufptr;
}

void* memmove(void* dstptr, const void* srcptr, size_t size) 
{
	unsigned char* dst = (unsigned char*) dstptr;
	const unsigned char* src = (const unsigned char*) srcptr;
	if (dst < src) 
    {
		for (size_t i = 0; i < size; i++)
			dst[i] = src[i];
	} 
    else 
    {
		for (size_t i = size; i != 0; i--)
			dst[i-1] = src[i-1];
	}
	return dstptr;
}

int memcmp(const void* aptr, const void* bptr, size_t size) 
{
	const unsigned char* a = (const unsigned char*) aptr;
	const unsigned char* b = (const unsigned char*) bptr;
	for (size_t i = 0; i < size; i++) 
    {
		if (a[i] < b[i])
			return -1;
		else if (b[i] < a[i])
			return 1;
	}
	return 0;
}

void* memcpy(void* dstptr, const void* srcptr, size_t size) 
{
	unsigned char* dst = (unsigned char*) dstptr;
	const unsigned char* src = (const unsigned char*) srcptr;
	for (size_t i = 0; i < size; i++)
	{
		dst[i] = src[i];
	}
	return dstptr;
}

size_t strlen(const char* str) 
{
	size_t len = 0;
	while (str[len])
		len++;
	return len;
}

char *strcpy(char * destination, const char * source)
{
	memcpy(destination, source, strlen(source));
	return destination;
}

}