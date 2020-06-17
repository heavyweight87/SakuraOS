#ifndef _KERNEL_TTY_H
#define _KERNEL_TTY_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

void terminal_init(void);
void terminal_putchar(char c);
void terminal_write(const char* data, size_t size);
void terminal_writestring(const char* data);
void terminal_switch_to_virtual();
void terminal_clear();

#ifdef __cplusplus
}
#endif

#endif
