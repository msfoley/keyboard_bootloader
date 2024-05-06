#ifndef BOOTLOADER_H
#define BOOTLOADER_H

#include <stdint.h>

#define BOOTLOADER_MAGIC 0x424F4F54

#define BOOTLOADER_FLAGS_ALLOW_MAJOR 0x00000001

__attribute__((packed))
struct bootloader_shared {
    uint32_t magic;
    uint32_t flags;
};

__attribute__((packed))
struct bootloader_info {
    uint32_t magic;
    uint8_t version_major;
    uint8_t version_minor;
    uint16_t reserved;
    uint32_t start_address;
    uint32_t length;
};

extern struct bootloader_shared bootloader_shared;
extern const struct bootloader_info bootloader_info;

#endif
