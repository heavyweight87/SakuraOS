#pragma once

#include <cstddef>

namespace Libk {

int memcmp(const void*, const void*, std::size_t);
void* memcpy(void* , const void* , std::size_t);
void* memmove(void*, const void*, std::size_t);
void* memset(void*, int, std::size_t);
std::size_t strlen(const char*);
char *strcpy(char * destination, const char * source);

}

