#ifndef UTIL_H
#define UTIL_H

#define container_of(pointer, type, member) \
    (type *)((char *) (pointer) - (char *) &((type *) 0)->member)

void boot_uart_init();
void boot_printf(const char *fmt, ...);

int boot_log2(uint32_t v);
int boot_log10(uint32_t v);

void us_delay(unsigned int us);

#endif
