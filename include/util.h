#ifndef UTIL_H
#define UTIL_H

#include <stdint.h>
#include <stdarg.h>

#define container_of(pointer, type, member) \
    (type *)((char *) (pointer) - (char *) &((type *) 0)->member)

void util_uart_init();
void util_printf(const char *fmt, ...);
int util_vsnprintf(char *buf, size_t len, const char *fmt, va_list args_in);

int util_log2(uint32_t v);
int util_log10(uint32_t v);

void util_us_delay(unsigned int us);
uint32_t util_get_core_freq();

#endif
