#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdarg.h>

#include <uart.h>
#include <device.h>

void util_printf(const char *fmt, ...) {
    va_list args;

    va_start(args, fmt);
    util_vsnprintf((char *) printf_buf, 256, fmt, args);
    va_end(args);

    char *buf = printf_buf;
    while (*buf != '\0') {
        if (*buf == '\n') {
            MXC_UART_WriteCharacter(MXC_UART_GET_UART(UART_NUMBER), '\r');
        }
        MXC_UART_WriteCharacter(MXC_UART_GET_UART(UART_NUMBER), *(buf++));
    }
}

uint32_t util_get_core_freq() {
    return SystemCoreClock;
}
