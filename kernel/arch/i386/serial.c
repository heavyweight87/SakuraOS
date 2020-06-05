

#include <kernel/io.h>
#include <limits.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>

#define PORT 0x3F8

static int is_transmit_empty() 
{
   return inb(PORT + 5) & 0x20;
}
 
void serial_printchar(char a) 
{
   while (is_transmit_empty() == 0);
   outb(PORT,a);
}

static void serial_print(const char* data, size_t length) 
{
	const unsigned char* bytes = (const unsigned char*) data;
	for (size_t i = 0; i < length; i++)
		serial_printchar(bytes[i]);
}

static char * itoa( int value, char * str, int base )
{
    char * rc;
    char * ptr;
    char * low;
    // Check for supported base.
    if ( base < 2 || base > 36 )
    {
        *str = '\0';
        return str;
    }
    rc = ptr = str;
    // Set '-' for negative decimals.
    if ( value < 0 && base == 10 )
    {
        *ptr++ = '-';
    }
    // Remember where the numbers start.
    low = ptr;
    // The actual conversion.
    do
    {
        // Modulo is negative for negative value. This trick makes abs() unnecessary.
        *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz"[35 + value % base];
        value /= base;
    } while ( value );
    // Terminating the string.
    *ptr-- = '\0';
    // Invert the numbers.
    while ( low < ptr )
    {
        char tmp = *low;
        *low++ = *ptr;
        *ptr-- = tmp;
    }
    return rc;
}

int serial_printf(const char* __restrict format, ...) 
{
	va_list parameters;
	va_start(parameters, format);

	int written = 0;

	while (*format != '\0') {
		size_t maxrem = INT_MAX - written;

		if (format[0] != '%' || format[1] == '%') {
			if (format[0] == '%')
				format++;
			size_t amount = 1;
			while (format[amount] && format[amount] != '%')
				amount++;
			if (maxrem < amount) {
				// TODO: Set errno to EOVERFLOW.
				return -1;
			}
			serial_print(format, amount);
			format += amount;
			written += amount;
			continue;
		}

		const char* format_begun_at = format++;

		if (*format == 'c') {
			format++;
			char c = (char) va_arg(parameters, int /* char promotes to int */);
			if (!maxrem) {
				// TODO: Set errno to EOVERFLOW.
				return -1;
			}
			serial_print(&c, sizeof(c));
			written++;
		} else if (*format == 's') {
			format++;
			const char* str = va_arg(parameters, const char*);
			size_t len = strlen(str);
			if (maxrem < len) {
				// TODO: Set errno to EOVERFLOW.
				return -1;
			}
			serial_print(str, len);
			written += len;
		} 
		else if(*format == 'd')
		{
			char buffer[50];
			format++;
			uint32_t c = va_arg(parameters, uint32_t /* char promotes to int */);
			if (!maxrem) {
				// TODO: Set errno to EOVERFLOW.
				return -1;
			}
			itoa(c, buffer, 10);
			serial_print(buffer, strlen(buffer));
			written++;
		}
		else {
			format = format_begun_at;
			size_t len = strlen(format);
			if (maxrem < len) {
				// TODO: Set errno to EOVERFLOW.
				return -1;
			}
			serial_print(format, len);
			written += len;
			format += len;
		}
	}

	va_end(parameters);
	return written;
}


void serial_init(void)
{
    outb(PORT + 1, 0x00);    // Disable all interrupts
    outb(PORT + 3, 0x80);    // Enable DLAB (set baud rate divisor)
    outb(PORT + 0, 0x03);    // Set divisor to 3 (lo byte) 38400 baud
    outb(PORT + 1, 0x00);    //                  (hi byte)
    outb(PORT + 3, 0x03);    // 8 bits, no parity, one stop bit
    outb(PORT + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
    outb(PORT + 4, 0x0B);    // IRQs enabled, RTS/DSR set
}