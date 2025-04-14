#ifndef UTIL_H
#define UTIL_H

#define container_of(pointer, type, member) \
    (type *)((char *) (pointer) - (char *) &((type *) 0)->member)

void util_uart_init();
void util_printf(const char *fmt, ...);

int util_log2(uint32_t v);
int util_log10(uint32_t v);

void util_us_delay(unsigned int us);
uint32_t util_get_core_freq();

#endif
