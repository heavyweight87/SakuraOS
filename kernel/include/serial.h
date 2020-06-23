#ifndef __KERNEL_SERIAL_H_
#define __KERNEL_SERIAL_H_

#ifdef __cplusplus
extern "C" {
#endif

void serial_init(void);
void serial_printchar(char a);
int serial_printf(const char* __restrict format, ...);
void serial_print(const char* data, size_t length);

#ifdef __cplusplus
}
#endif

#endif //__KERNEL_SERIAL_H_
