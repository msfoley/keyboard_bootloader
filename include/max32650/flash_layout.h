#ifndef FLASH_LAYOUT_H
#define FLASH_LAYOUT_H


#define FLASH_SIZE 0x300000
#define FLASH_START 0x10000000

#define PAGE_SIZE 0x4000
#define PAGE_COUNT (FLASH_SIZE / PAGE_SIZE)
#define PAGE_ALIGN(x) (x & ~(PAGE_SIZE - 1))

#define BOOTLOADER_SIZE PAGE_SIZE
#define BOOTLOADER_START FLASH_START

#define APP_SIZE (FLASH_SIZE - BOOTLOADER_SIZE)
#define APP_START (BOOTLOADER_START + BOOTLOADER_START)

#endif
