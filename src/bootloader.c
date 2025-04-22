#include <stdint.h>

#include <util.h>
#include <device.h>
#include <bootloader.h>
#include <config.h>

extern uint32_t __flash_len;

__attribute__((section(".bootloader_shared")))
struct bootloader_shared bootloader_shared;

__attribute__((section(".bootloader_info")))
const struct bootloader_info bootloader_info = {
    .magic = BOOTLOADER_MAGIC,
    .version_major = VERSION_MAJOR,
    .version_minor = VERSION_MINOR,
    .start_address = BOOTLOADER_START,
    .length = (uint32_t) &__flash_len,
    .checksum = 0xFFFFFFFF
};

uint32_t calculate_image_crc32(uint32_t address) {
    uint8_t *buf = (uint8_t *) address;
    uint32_t crc = 0;
    uint32_t default_checksum = 0xFFFFFFFF;
    uint32_t checksum_offset = BOOTLOADER_INFO_OFFSET + offsetof(struct bootloader_info, checksum);
    struct bootloader_info *image_info = (struct bootloader_info *) (buf + BOOTLOADER_INFO_OFFSET);

    crc = util_crc32(crc, buf, checksum_offset);
    crc = util_crc32(crc, (uint8_t *) &default_checksum, 4);
    crc = util_crc32(crc, buf + checksum_offset + 4, image_info->length - (checksum_offset + 4));

    return crc;
}

__attribute__((naked,noreturn))
void jump_asm(uint32_t pc, uint32_t sp) {
    __asm("msr msp, r1");
    __asm("bx r0");
}

__attribute__((noreturn))
void jump(uint32_t address) {
    __disable_irq();

    NVIC->ICER[0] = 0xFFFFFFFF;
    NVIC->ICER[0] = 0xFFFFFFFF;
    NVIC->ICER[2] = 0xFFFFFFFF;
    NVIC->ICER[3] = 0xFFFFFFFF;
    NVIC->ICER[4] = 0xFFFFFFFF;
    NVIC->ICER[5] = 0xFFFFFFFF;
    NVIC->ICER[6] = 0xFFFFFFFF;
    NVIC->ICER[7] = 0xFFFFFFFF;
    
    NVIC->ICPR[0] = 0xFFFFFFFF;
    NVIC->ICPR[1] = 0xFFFFFFFF;
    NVIC->ICPR[2] = 0xFFFFFFFF;
    NVIC->ICPR[3] = 0xFFFFFFFF;
    NVIC->ICPR[4] = 0xFFFFFFFF;
    NVIC->ICPR[5] = 0xFFFFFFFF;
    NVIC->ICPR[6] = 0xFFFFFFFF;
    NVIC->ICPR[7] = 0xFFFFFFFF;

    SysTick->CTRL = 0;
    SCB->ICSR |= SCB_ICSR_PENDSTCLR_Msk;
    SCB->ICSR |= SCB_ICSR_PENDSVCLR_Msk;

    jump_device(address);

    SCB->VTOR = address;

    jump_asm(((uint32_t *) address)[1], ((uint32_t *) address)[0]);

    while (1);
}
