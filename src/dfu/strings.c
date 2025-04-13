#include <stdint.h>

#include <dfu/dfu.h>
#include "strings.h"

uint8_t dfu_status_msg[DFU_STATUS_LEN][DFU_STRINGS_MAX_LEN] = {
    { 0x3E, 0x03, 'N', 0x00, 'o', 0x00, ' ', 0x00, 'e', 0x00, 'r', 0x00, 'r', 0x00, 'o', 0x00, 'r', 0x00, ' ', 0x00, 'c', 0x00, 'o', 0x00, 'n', 0x00, 'd', 0x00, 'i', 0x00, 't', 0x00, 'i', 0x00, 'o', 0x00, 'n', 0x00, ' ', 0x00, 'i', 0x00, 's', 0x00, ' ', 0x00, 'p', 0x00, 'r', 0x00, 'e', 0x00, 's', 0x00, 'e', 0x00, 'n', 0x00, 't', 0x00, '.', 0x00 },
    { 0x5A, 0x03, 'F', 0x00, 'i', 0x00, 'l', 0x00, 'e', 0x00, ' ', 0x00, 'i', 0x00, 's', 0x00, ' ', 0x00, 'n', 0x00, 'o', 0x00, 't', 0x00, ' ', 0x00, 't', 0x00, 'a', 0x00, 'r', 0x00, 'g', 0x00, 'e', 0x00, 't', 0x00, 'e', 0x00, 'd', 0x00, ' ', 0x00, 'f', 0x00, 'o', 0x00, 'r', 0x00, ' ', 0x00, 'u', 0x00, 's', 0x00, 'e', 0x00, ' ', 0x00, 'b', 0x00, 'y', 0x00, ' ', 0x00, 't', 0x00, 'h', 0x00, 'i', 0x00, 's', 0x00, ' ', 0x00, 'd', 0x00, 'e', 0x00, 'v', 0x00, 'i', 0x00, 'c', 0x00, 'e', 0x00, '.', 0x00 },
    { 0x94, 0x03, 'F', 0x00, 'i', 0x00, 'l', 0x00, 'e', 0x00, ' ', 0x00, 'i', 0x00, 's', 0x00, ' ', 0x00, 'f', 0x00, 'o', 0x00, 'r', 0x00, ' ', 0x00, 't', 0x00, 'h', 0x00, 'i', 0x00, 's', 0x00, ' ', 0x00, 'd', 0x00, 'e', 0x00, 'v', 0x00, 'i', 0x00, 'c', 0x00, 'e', 0x00, ' ', 0x00, 'b', 0x00, 'u', 0x00, 't', 0x00, ' ', 0x00, 'f', 0x00, 'a', 0x00, 'i', 0x00, 'l', 0x00, 's', 0x00, ' ', 0x00, 's', 0x00, 'o', 0x00, 'm', 0x00, 'e', 0x00, ' ', 0x00, 'v', 0x00, 'e', 0x00, 'n', 0x00, 'd', 0x00, 'o', 0x00, 'r', 0x00, '-', 0x00, 's', 0x00, 'p', 0x00, 'e', 0x00, 'c', 0x00, 'i', 0x00, 'f', 0x00, 'i', 0x00, 'c', 0x00, ' ', 0x00, 'v', 0x00, 'e', 0x00, 'r', 0x00, 'i', 0x00, 'f', 0x00, 'i', 0x00, 'c', 0x00, 'a', 0x00, 't', 0x00, 'i', 0x00, 'o', 0x00, 'n', 0x00, ' ', 0x00, 't', 0x00, 'e', 0x00, 's', 0x00, 't', 0x00, '.', 0x00 },
    { 0x44, 0x03, 'D', 0x00, 'e', 0x00, 'v', 0x00, 'i', 0x00, 'c', 0x00, 'e', 0x00, ' ', 0x00, 'i', 0x00, 's', 0x00, ' ', 0x00, 'u', 0x00, 'n', 0x00, 'a', 0x00, 'b', 0x00, 'l', 0x00, 'e', 0x00, ' ', 0x00, 't', 0x00, 'o', 0x00, ' ', 0x00, 'w', 0x00, 'r', 0x00, 'i', 0x00, 't', 0x00, 'e', 0x00, ' ', 0x00, 'm', 0x00, 'e', 0x00, 'm', 0x00, 'o', 0x00, 'r', 0x00, 'y', 0x00, '.', 0x00 },
    { 0x3C, 0x03, 'M', 0x00, 'e', 0x00, 'm', 0x00, 'o', 0x00, 'r', 0x00, 'y', 0x00, ' ', 0x00, 'e', 0x00, 'r', 0x00, 'a', 0x00, 's', 0x00, 'e', 0x00, ' ', 0x00, 'f', 0x00, 'u', 0x00, 'n', 0x00, 'c', 0x00, 't', 0x00, 'i', 0x00, 'o', 0x00, 'n', 0x00, ' ', 0x00, 'f', 0x00, 'a', 0x00, 'i', 0x00, 'l', 0x00, 'e', 0x00, 'd', 0x00, '.', 0x00 },
    { 0x36, 0x03, 'M', 0x00, 'e', 0x00, 'm', 0x00, 'o', 0x00, 'r', 0x00, 'y', 0x00, ' ', 0x00, 'e', 0x00, 'r', 0x00, 'a', 0x00, 's', 0x00, 'e', 0x00, ' ', 0x00, 'c', 0x00, 'h', 0x00, 'e', 0x00, 'c', 0x00, 'k', 0x00, ' ', 0x00, 'f', 0x00, 'a', 0x00, 'i', 0x00, 'l', 0x00, 'e', 0x00, 'd', 0x00, '.', 0x00 },
    { 0x40, 0x03, 'P', 0x00, 'r', 0x00, 'o', 0x00, 'g', 0x00, 'r', 0x00, 'a', 0x00, 'm', 0x00, ' ', 0x00, 'm', 0x00, 'e', 0x00, 'm', 0x00, 'o', 0x00, 'r', 0x00, 'y', 0x00, ' ', 0x00, 'f', 0x00, 'u', 0x00, 'n', 0x00, 'c', 0x00, 't', 0x00, 'i', 0x00, 'o', 0x00, 'n', 0x00, ' ', 0x00, 'f', 0x00, 'a', 0x00, 'i', 0x00, 'l', 0x00, 'e', 0x00, 'd', 0x00, '.', 0x00 },
    { 0x46, 0x03, 'P', 0x00, 'r', 0x00, 'o', 0x00, 'g', 0x00, 'r', 0x00, 'a', 0x00, 'm', 0x00, 'm', 0x00, 'e', 0x00, 'd', 0x00, ' ', 0x00, 'm', 0x00, 'e', 0x00, 'm', 0x00, 'o', 0x00, 'r', 0x00, 'y', 0x00, ' ', 0x00, 'f', 0x00, 'u', 0x00, 'n', 0x00, 'c', 0x00, 't', 0x00, 'i', 0x00, 'o', 0x00, 'n', 0x00, ' ', 0x00, 'f', 0x00, 'a', 0x00, 'i', 0x00, 'l', 0x00, 'e', 0x00, 'd', 0x00, '.', 0x00 },
    { 0x88, 0x03, 'C', 0x00, 'a', 0x00, 'n', 0x00, 'n', 0x00, 'o', 0x00, 't', 0x00, ' ', 0x00, 'p', 0x00, 'r', 0x00, 'o', 0x00, 'g', 0x00, 'r', 0x00, 'a', 0x00, 'm', 0x00, ' ', 0x00, 'm', 0x00, 'e', 0x00, 'm', 0x00, 'o', 0x00, 'r', 0x00, 'y', 0x00, ' ', 0x00, 'd', 0x00, 'u', 0x00, 'e', 0x00, ' ', 0x00, 't', 0x00, 'o', 0x00, ' ', 0x00, 'r', 0x00, 'e', 0x00, 'c', 0x00, 'e', 0x00, 'i', 0x00, 'v', 0x00, 'e', 0x00, 'd', 0x00, ' ', 0x00, 'a', 0x00, 'd', 0x00, 'd', 0x00, 'r', 0x00, 'e', 0x00, 's', 0x00, 's', 0x00, ' ', 0x00, 't', 0x00, 'h', 0x00, 'a', 0x00, 't', 0x00, ' ', 0x00, 'i', 0x00, 's', 0x00, ' ', 0x00, 'o', 0x00, 'u', 0x00, 't', 0x00, ' ', 0x00, 'o', 0x00, 'f', 0x00, ' ', 0x00, 'r', 0x00, 'a', 0x00, 'n', 0x00, 'g', 0x00, 'e', 0x00, '.', 0x00 },
    { 0xB8, 0x03, 'R', 0x00, 'e', 0x00, 'c', 0x00, 'e', 0x00, 'i', 0x00, 'v', 0x00, 'e', 0x00, 'd', 0x00, ' ', 0x00, 'D', 0x00, 'F', 0x00, 'U', 0x00, '_', 0x00, 'D', 0x00, 'N', 0x00, 'L', 0x00, 'O', 0x00, 'A', 0x00, 'D', 0x00, ' ', 0x00, 'w', 0x00, 'i', 0x00, 't', 0x00, 'h', 0x00, ' ', 0x00, 'w', 0x00, 'L', 0x00, 'e', 0x00, 'n', 0x00, 'g', 0x00, 't', 0x00, 'h', 0x00, ' ', 0x00, '=', 0x00, ' ', 0x00, '0', 0x00, ',', 0x00, ' ', 0x00, 'b', 0x00, 'u', 0x00, 't', 0x00, ' ', 0x00, 'd', 0x00, 'e', 0x00, 'v', 0x00, 'i', 0x00, 'c', 0x00, 'e', 0x00, ' ', 0x00, 'd', 0x00, 'o', 0x00, 'e', 0x00, 's', 0x00, ' ', 0x00, 'n', 0x00, 'o', 0x00, 't', 0x00, ' ', 0x00, 't', 0x00, 'h', 0x00, 'i', 0x00, 'n', 0x00, 'k', 0x00, ' ', 0x00, 'i', 0x00, 't', 0x00, ' ', 0x00, 'h', 0x00, 'a', 0x00, 's', 0x00, ' ', 0x00, 'a', 0x00, 'l', 0x00, 'l', 0x00, ' ', 0x00, 'o', 0x00, 'f', 0x00, ' ', 0x00, 't', 0x00, 'h', 0x00, 'e', 0x00, ' ', 0x00, 'd', 0x00, 'a', 0x00, 't', 0x00, 'a', 0x00, ' ', 0x00, 'y', 0x00, 'e', 0x00, 't', 0x00, '.', 0x00 },
    { 0xA2, 0x03, 'D', 0x00, 'e', 0x00, 'v', 0x00, 'i', 0x00, 'c', 0x00, 'e', 0x00, '\'', 0x00, 's', 0x00, ' ', 0x00, 'f', 0x00, 'i', 0x00, 'r', 0x00, 'm', 0x00, 'w', 0x00, 'a', 0x00, 'r', 0x00, 'e', 0x00, ' ', 0x00, 'i', 0x00, 's', 0x00, ' ', 0x00, 'c', 0x00, 'o', 0x00, 'r', 0x00, 'r', 0x00, 'u', 0x00, 'p', 0x00, 't', 0x00, '.', 0x00, ' ', 0x00, 'I', 0x00, 't', 0x00, ' ', 0x00, 'c', 0x00, 'a', 0x00, 'n', 0x00, 'n', 0x00, 'o', 0x00, 't', 0x00, ' ', 0x00, 'r', 0x00, 'e', 0x00, 't', 0x00, 'u', 0x00, 'r', 0x00, 'n', 0x00, ' ', 0x00, 't', 0x00, 'o', 0x00, ' ', 0x00, 'r', 0x00, 'u', 0x00, 'n', 0x00, '-', 0x00, 't', 0x00, 'i', 0x00, 'm', 0x00, 'e', 0x00, ' ', 0x00, '(', 0x00, 'n', 0x00, 'o', 0x00, 'n', 0x00, '-', 0x00, 'D', 0x00, 'F', 0x00, 'U', 0x00, ')', 0x00, ' ', 0x00, 'o', 0x00, 'p', 0x00, 'e', 0x00, 'r', 0x00, 'a', 0x00, 't', 0x00, 'i', 0x00, 'o', 0x00, 'n', 0x00, 's', 0x00, '.', 0x00 },
    { 0x56, 0x03, 'i', 0x00, 'S', 0x00, 't', 0x00, 'r', 0x00, 'i', 0x00, 'n', 0x00, 'g', 0x00, ' ', 0x00, 'i', 0x00, 'n', 0x00, 'd', 0x00, 'i', 0x00, 'c', 0x00, 'a', 0x00, 't', 0x00, 'e', 0x00, 's', 0x00, ' ', 0x00, 'a', 0x00, ' ', 0x00, 'v', 0x00, 'e', 0x00, 'n', 0x00, 'd', 0x00, 'o', 0x00, 'r', 0x00, '-', 0x00, 's', 0x00, 'p', 0x00, 'e', 0x00, 'c', 0x00, 'i', 0x00, 'f', 0x00, 'i', 0x00, 'c', 0x00, ' ', 0x00, 'e', 0x00, 'r', 0x00, 'r', 0x00, 'o', 0x00, 'r', 0x00, '.', 0x00 },
    { 0x60, 0x03, 'D', 0x00, 'e', 0x00, 'v', 0x00, 'i', 0x00, 'c', 0x00, 'e', 0x00, ' ', 0x00, 'd', 0x00, 'e', 0x00, 't', 0x00, 'e', 0x00, 'c', 0x00, 't', 0x00, 'e', 0x00, 'd', 0x00, ' ', 0x00, 'u', 0x00, 'n', 0x00, 'e', 0x00, 's', 0x00, 'p', 0x00, 'e', 0x00, 'c', 0x00, 't', 0x00, 'e', 0x00, 'd', 0x00, ' ', 0x00, 'U', 0x00, 'S', 0x00, 'B', 0x00, ' ', 0x00, 'r', 0x00, 'e', 0x00, 's', 0x00, 'e', 0x00, 't', 0x00, ' ', 0x00, 's', 0x00, 'i', 0x00, 'g', 0x00, 'n', 0x00, 'a', 0x00, 'l', 0x00, 'i', 0x00, 'n', 0x00, 'g', 0x00, '.', 0x00 },
    { 0x56, 0x03, 'D', 0x00, 'e', 0x00, 'v', 0x00, 'i', 0x00, 'c', 0x00, 'e', 0x00, ' ', 0x00, 'd', 0x00, 'e', 0x00, 't', 0x00, 'e', 0x00, 'c', 0x00, 't', 0x00, 'e', 0x00, 'd', 0x00, ' ', 0x00, 'u', 0x00, 'n', 0x00, 'e', 0x00, 'x', 0x00, 'p', 0x00, 'e', 0x00, 'c', 0x00, 't', 0x00, 'e', 0x00, 'd', 0x00, ' ', 0x00, 'p', 0x00, 'o', 0x00, 'w', 0x00, 'e', 0x00, 'r', 0x00, ' ', 0x00, 'o', 0x00, 'n', 0x00, ' ', 0x00, 'r', 0x00, 'e', 0x00, 's', 0x00, 'e', 0x00, 't', 0x00, '.', 0x00 },
    { 0x80, 0x03, 'S', 0x00, 'o', 0x00, 'm', 0x00, 'e', 0x00, 't', 0x00, 'h', 0x00, 'i', 0x00, 'n', 0x00, 'g', 0x00, ' ', 0x00, 'w', 0x00, 'e', 0x00, 'n', 0x00, 't', 0x00, ' ', 0x00, 'w', 0x00, 'r', 0x00, 'o', 0x00, 'n', 0x00, 'g', 0x00, ',', 0x00, ' ', 0x00, 'b', 0x00, 'u', 0x00, 't', 0x00, ' ', 0x00, 't', 0x00, 'h', 0x00, 'e', 0x00, ' ', 0x00, 'd', 0x00, 'e', 0x00, 'v', 0x00, 'i', 0x00, 'c', 0x00, 'e', 0x00, ' ', 0x00, 'd', 0x00, 'o', 0x00, 'e', 0x00, 's', 0x00, ' ', 0x00, 'n', 0x00, 'o', 0x00, 't', 0x00, ' ', 0x00, 'k', 0x00, 'n', 0x00, 'o', 0x00, 'w', 0x00, ' ', 0x00, 'w', 0x00, 'h', 0x00, 'a', 0x00, 't', 0x00, ' ', 0x00, 'i', 0x00, 't', 0x00, ' ', 0x00, 'w', 0x00, 'a', 0x00, 's', 0x00, '.', 0x00 },
    { 0x4C, 0x03, 'D', 0x00, 'e', 0x00, 'v', 0x00, 'i', 0x00, 'c', 0x00, 'e', 0x00, ' ', 0x00, 's', 0x00, 't', 0x00, 'a', 0x00, 'l', 0x00, 'l', 0x00, 'e', 0x00, 'd', 0x00, ' ', 0x00, 'a', 0x00, 'n', 0x00, ' ', 0x00, 'u', 0x00, 'n', 0x00, 'e', 0x00, 'x', 0x00, 'p', 0x00, 'e', 0x00, 'c', 0x00, 't', 0x00, 'e', 0x00, 'd', 0x00, ' ', 0x00, 'r', 0x00, 'e', 0x00, 'q', 0x00, 'u', 0x00, 'e', 0x00, 's', 0x00, 't', 0x00, '.', 0x00 },
};
