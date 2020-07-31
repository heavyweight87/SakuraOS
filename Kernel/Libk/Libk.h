#pragma once

#include <cstddef>
#include <cstdarg>
#include "TTY.h"

namespace Libk {

#ifdef __cplusplus
extern "C" {
#endif

//stdio
void setTTY(Devices::TTY *tty);
int putchar(int);
int puts(const char*);
void printk(const char* __restrict format, ...);
int sprintf(char* buffer, const char* format, ...);
int snprintf(char* buffer, size_t count, const char* format, ...);
int vsnprintf(char* buffer, size_t count, const char* format, va_list va);

//string
int memcmp(const void*, const void*, std::size_t);
void* memcpy(void* , const void* , std::size_t);
void* memmove(void*, const void*, std::size_t);
void* memset(void*, int, std::size_t);
std::size_t strlen(const char*);
char *strcpy(char * destination, const char * source);

#ifdef __cplusplus
}
#endif

}
