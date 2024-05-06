#include <stdint.h>

#include <bootloader.h>
#include <config.h>

__attribute__((section(".flash_isr")))
struct bootloader_shared bootloader_shared = {
    .magic = 0,
    .flags = 0
};

extern uint32_t _text;
extern uint32_t __flash_len;

__attribute__((section(".bootloader_info")))
const struct bootloader_info bootloader_info = {
    .magic = BOOTLOADER_MAGIC,
    .version_major = VERSION_MAJOR,
    .version_minor = VERSION_MINOR,
    .start_address = (uint32_t) &_text,
    .length = (uint32_t) &__flash_len,
};
