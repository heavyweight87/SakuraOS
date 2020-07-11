#pragma once

namespace Libk {

#ifdef __cplusplus
extern "C" {
#endif

int putchar(int);
int puts(const char*);

void printk(const char* __restrict format, ...);

#ifdef __cplusplus
}
#endif

}
