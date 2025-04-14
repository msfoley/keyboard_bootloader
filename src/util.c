#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdarg.h>

#include <uart.h>
#include <device.h>

char printf_buf[256];

void boot_uart_init() {
    MXC_UART_Init(MXC_UART_GET_UART(UART_NUMBER), 115200);
}

static const int MultiplyDeBruijnBitPosition[32] = 
{
  0, 9, 1, 10, 13, 21, 2, 29, 11, 14, 16, 18, 22, 25, 3, 30,
  8, 12, 20, 28, 15, 17, 24, 7, 19, 27, 23, 6, 26, 5, 4, 31
};

int boot_log2(uint32_t v) {
    v |= v >> 1; // first round down to one less than a power of 2 
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;

    return MultiplyDeBruijnBitPosition[(uint32_t)(v * 0x07C4ACDDU) >> 27];
}

static unsigned int const power10[] = { 1, 10, 100, 1000, 10000, 100000, 1000000,
                                        10000000, 100000000, 1000000000 };

int boot_log10(uint32_t v) {
    int t = (boot_log2(v) + 1) * 1233 > 12;

    return t - (v < power10[t]);
}

size_t boot_strlen(char *buf) {
    size_t count = 0;

    while (buf[count++] != '\0');

    return count - 1;
}

int boot_vsnprintf(char *buf, size_t len, const char *fmt, va_list args_in) {
    const char *fmt_ptr = fmt;
    char *buf_ptr = buf;
    int i;
    int size = 0;
    va_list args;
    size_t slen;
    char int_buf[20];

    va_copy(args, args_in);
    while (*fmt_ptr != '\0') {
        if (*fmt_ptr != '%') {
            if ((size + 1) >= (len - 1)) {
                break;
            }

            *buf_ptr = *fmt_ptr;
            buf_ptr++;
            fmt_ptr++;
            size += 1;
            continue;
        }
        fmt_ptr++; // '%' found

        // Get zero padding
        int zero_pad = 0;
        if (*fmt_ptr == '0') {
            fmt_ptr++;

            while ((*fmt_ptr <= '9') && (*fmt_ptr >= '0')) {
                zero_pad *= 10;
                zero_pad += *fmt_ptr - '0';
                fmt_ptr++;
            }

            if (*fmt_ptr == '\0') {
                break;
            }
        }

        if (*fmt_ptr == 's') {
            char *s = va_arg(args, char *);

            fmt_ptr++;
            slen = boot_strlen(s);

            if ((size + slen) >= (len - 1)) {
                break;
            }

            memcpy(buf_ptr, s, slen);
            buf_ptr += slen;
            size += slen;
            continue;
        }

        int neg = 0;
        int base = 16;
        int hex_upper = 1;
        int int_len = 0;
        uint32_t val = 0;

        int vi;
        unsigned int vu;
        uint32_t vx;
        // Get format specifier
        switch (*fmt_ptr) {
            case 'd':
                vi = va_arg(args, int);
                if (vi < 0) {
                    neg = 1;
                }
                val = -vi;
                base = 10;
                break;
            case 'u':
                vu = va_arg(args, unsigned int);
                base = 10;
                val = vu;
                break;
            case 'x':
                hex_upper = 0;
            case 'X':
            case 'p':
                vx = va_arg(args, uint32_t);
                val = vx;
                break;
            default:
                if (*fmt_ptr != '\0') {
                    fmt_ptr++;
                }
                continue;
        }

        if (val != 0) {
            if (base == 10) {
                int_len = boot_log10(val);
            } else {
                int_len = boot_log2(val) / 4;
            }
        } else {
            int_len = 0;
        }

        if (zero_pad > 16) {
            zero_pad = 16;
        }

        char *int_ptr = int_buf;
        if (neg) {
            *int_ptr = '-';
            int_ptr++;
        }
        if ((int_len + 1) < zero_pad) {
            memset(int_ptr, '0', zero_pad - (int_len + 1));
            int_ptr += zero_pad - (int_len + 1);
        }

        if (base == 10) {
            for (i = int_len; i >= 0; i--) {
                uint32_t v = (val / power10[i]) % 10;
                *(int_ptr++) = '0' + v;
            }
        } else {
            for (i = int_len; i >= 0; i--) {
                uint32_t v = (val >> (4 * i)) & 0x0F;
                char c;

                if (v < 10) {
                    c = '0' + v;
                } else {
                    c = hex_upper ? 'A' : 'a';
                    c += v - 10;
                }
                *(int_ptr++) = c;
            }
        }
        *int_ptr = '\0';

        slen = boot_strlen(int_buf);
        if ((size + slen) >= (len - 1)) {
            break;
        }

        memcpy(buf_ptr, int_buf, slen);
        buf_ptr += slen;
        size += slen;

        fmt_ptr++;
    }
    va_end(args);

    *buf_ptr = '\0';

    return size;
}

void boot_printf(const char *fmt, ...) {
    va_list args;

    va_start(args, fmt);
    boot_vsnprintf((char *) printf_buf, 256, fmt, args);
    va_end(args);

    char *buf = printf_buf;
    while (*buf != '\0') {
        if (*buf == '\n') {
            MXC_UART_WriteCharacter(MXC_UART_GET_UART(UART_NUMBER), '\r');
        }
        MXC_UART_WriteCharacter(MXC_UART_GET_UART(UART_NUMBER), *(buf++));
    }
}
