#include <stdint.h>

#include <gcr_regs.h>

#include <device.h>
#include <bootloader.h>
#include <config.h>

extern uint32_t __flash_len;

__attribute__((section(".bootloader_shared")))
struct bootloader_shared bootloader_shared = {
    .magic = 0,
    .flags = 0
};

__attribute__((section(".bootloader_info")))
const struct bootloader_info bootloader_info = {
    .magic = BOOTLOADER_MAGIC,
    .version_major = VERSION_MAJOR,
    .version_minor = VERSION_MINOR,
    .start_address = BOOTLOADER_START,
    .length = (uint32_t) &__flash_len,
};

__attribute__((naked,noreturn))
void jump_asm(uint32_t pc, uint32_t sp) {
    __asm("msr msp, r1");
    __asm("bx r0");
}

__attribute__((noreturn))
void jump(uint32_t address) {
    MXC_GCR->pclk_dis0 = 0xFFFFFFFF;
    MXC_GCR->pclk_dis1 = 0xFFFFFFFF;

    NVIC->ICER[0] = 0xFFFFFFFF;
    NVIC->ICER[1] = 0xFFFFFFFF;
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

    SCB->VTOR = address;

    jump_asm(((uint32_t *) address)[0], ((uint32_t *) address)[1]);

    while (1);
}
