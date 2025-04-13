#include <stdint.h>

#include "nvic_table.h"
#include "mxc_sys.h"
#include "flc_regs.h"
#include "icc_regs.h"
#include "mxc_errors.h"

#define ISR_VECTOR_SIZE_WORD 96

#ifndef __STACK_SIZE
#define __STACK_SIZE 0x1000
#endif

#ifndef __HEAP_SIZE
#define __HEAP_SIZE 0x1000
#endif

#define WEAK_FLASH_ISR(x) \
    __attribute__((section(".flash_isr"), weak)) \
    void x() { \
        while (1); \
    }

#define WEAK_ISR(x) \
    __attribute__((weak)) \
    void x() { \
        while (1); \
    }

extern uint32_t SystemCoreClock;
extern uint8_t ChipRevision;

extern uint32_t __stack_top;
extern uint32_t __load_data;
extern uint32_t _data;
extern uint32_t __data_len;
extern uint32_t _bss;
extern uint32_t __bss_len;

WEAK_ISR(nmi_handler);
WEAK_ISR(hard_fault_handler);
WEAK_ISR(mem_manage_handler);
WEAK_ISR(bus_fault_handler);
WEAK_ISR(usage_fault_handler);

extern int main();

void init_system();

__attribute__((naked, section(".flash_isr")))
void flash_reset_handler() {
    uint32_t i;
    uint32_t *dst;
    uint32_t *src;
    uint32_t len;

    asm volatile ("    \n\
           ldr r0, =__stack_top\n\
           msr msp, r0\n\
    ");

    /* Workaround: Write to SCON register on power up to fix trim issue for SRAM */
    MXC_GCR->scon = (MXC_GCR->scon & ~(MXC_F_GCR_SCON_OVR)) | (MXC_S_GCR_SCON_OVR_1V1);

    src = &__load_data;
    dst = &_data;
    len = ((uint32_t) &__data_len) >> 2;
    for (i = 0; i < len; i++) {
        dst[i] = src[i];
    }
    
    dst = &_bss;
    len = ((uint32_t) &__bss_len) >> 2;
    for (i = 0; i < len; i++) {
        dst[i] = 0x00000000;
    }

    NVIC_SetRAM();

    dst = (uint32_t *) SCB->VTOR;
    dst[NonMaskableInt_IRQn + 16] = (uint32_t) nmi_handler;
    dst[HardFault_IRQn + 16] = (uint32_t) hard_fault_handler;
    dst[MemoryManagement_IRQn + 16] = (uint32_t) mem_manage_handler;
    dst[BusFault_IRQn + 16] = (uint32_t) bus_fault_handler;
    dst[UsageFault_IRQn + 16] = (uint32_t) usage_fault_handler;

    init_system();
    main();

    while (1);
}

WEAK_FLASH_ISR(flash_nmi_handler);
WEAK_FLASH_ISR(flash_hard_fault_handler);
WEAK_FLASH_ISR(flash_mem_manage_handler);
WEAK_FLASH_ISR(flash_bus_fault_handler);
WEAK_FLASH_ISR(flash_usage_fault_handler);

WEAK_ISR(svc_handler);
WEAK_ISR(debugmon_handler);
WEAK_ISR(pendsv_handler);
WEAK_ISR(systick_handler);
WEAK_ISR(pf_handler);
WEAK_ISR(wdt0_handler);
WEAK_ISR(usb_handler);
WEAK_ISR(rtc_handler);
WEAK_ISR(trng_handler);
WEAK_ISR(tmr0_handler);
WEAK_ISR(tmr1_handler);
WEAK_ISR(tmr2_handler);
WEAK_ISR(tmr3_handler);
WEAK_ISR(tmr4_handler);
WEAK_ISR(tmr5_handler);
WEAK_ISR(rsv11_handler);
WEAK_ISR(lcd_handler);
WEAK_ISR(i2c0_handler);
WEAK_ISR(uart0_handler);
WEAK_ISR(uart1_handler);
WEAK_ISR(spi0_handler);
WEAK_ISR(spi1_handler);
WEAK_ISR(spi2_handler);
WEAK_ISR(rsv19_handler);
WEAK_ISR(adc_handler);
WEAK_ISR(rsv21_handler);
WEAK_ISR(rsv22_handler);
WEAK_ISR(flc_handler);
WEAK_ISR(gpio0_handler);
WEAK_ISR(gpio1_handler);
WEAK_ISR(gpio2_handler);
WEAK_ISR(crypto_handler);
WEAK_ISR(dma0_handler);
WEAK_ISR(dma1_handler);
WEAK_ISR(dma2_handler);
WEAK_ISR(dma3_handler);
WEAK_ISR(rsv32_handler);
WEAK_ISR(rsv33_handler);
WEAK_ISR(uart2_handler);
WEAK_ISR(rsv35_handler);
WEAK_ISR(i2c1_handler);
WEAK_ISR(rsv37_handler);
WEAK_ISR(spixc_handler);
WEAK_ISR(rsv39_handler);
WEAK_ISR(rsv40_handler);
WEAK_ISR(rsv41_handler);
WEAK_ISR(rsv42_handler);
WEAK_ISR(rsv43_handler);
WEAK_ISR(rsv44_handler);
WEAK_ISR(rsv45_handler);
WEAK_ISR(rsv46_handler);
WEAK_ISR(rsv47_handler);
WEAK_ISR(rsv48_handler);
WEAK_ISR(rsv49_handler);
WEAK_ISR(rsv50_handler);
WEAK_ISR(rsv51_handler);
WEAK_ISR(rsv52_handler);
WEAK_ISR(rsv53_handler);
WEAK_ISR(gpiowake_handler);
WEAK_ISR(rsv55_handler);
WEAK_ISR(spi3_handler);
WEAK_ISR(wdt1_handler);
WEAK_ISR(gpio3_handler);
WEAK_ISR(pt_handler);
WEAK_ISR(sdma_handler);
WEAK_ISR(hpb_handler);
WEAK_ISR(rsv62_handler);
WEAK_ISR(rsv63_handler);
WEAK_ISR(rsv64_handler);
WEAK_ISR(rsv65_handler);
WEAK_ISR(sdhc_handler);
WEAK_ISR(owm_handler);
WEAK_ISR(dma4_handler);
WEAK_ISR(dma5_handler);
WEAK_ISR(dma6_handler);
WEAK_ISR(dma7_handler);
WEAK_ISR(dma8_handler);
WEAK_ISR(dma9_handler);
WEAK_ISR(dma10_handler);
WEAK_ISR(dma11_handler);
WEAK_ISR(dma12_handler);
WEAK_ISR(dma13_handler);
WEAK_ISR(dma14_handler);
WEAK_ISR(dma15_handler);
WEAK_ISR(usbdma_handler);

__attribute__((section(".stack")))
uint8_t stack[__STACK_SIZE];

__attribute__((section(".heap")))
uint8_t heap[__HEAP_SIZE];

__attribute__((section(".isr_vector")))
void (* __isr_vector[MXC_IRQ_COUNT])(void) = {
    (void *) &__stack_top,
    flash_reset_handler,
    flash_nmi_handler,
    flash_hard_fault_handler,
    flash_mem_manage_handler,
    flash_bus_fault_handler,
    flash_usage_fault_handler,
    0,
    0,
    0,
    0,
    svc_handler,
    debugmon_handler,
    0,
    pendsv_handler,
    systick_handler,
    pf_handler,
    wdt0_handler,
    usb_handler,
    rtc_handler,
    trng_handler,
    tmr0_handler,
    tmr1_handler,
    tmr2_handler,
    tmr3_handler,
    tmr4_handler,
    tmr5_handler,
    rsv11_handler,
    lcd_handler,
    i2c0_handler,
    uart0_handler,
    uart1_handler,
    spi0_handler,
    spi1_handler,
    spi2_handler,
    rsv19_handler,
    adc_handler,
    rsv21_handler,
    rsv22_handler,
    flc_handler,
    gpio0_handler,
    gpio1_handler,
    gpio2_handler,
    crypto_handler,
    dma0_handler,
    dma1_handler,
    dma2_handler,
    dma3_handler,
    rsv32_handler,
    rsv33_handler,
    uart2_handler,
    rsv35_handler,
    i2c1_handler,
    rsv37_handler,
    spixc_handler,
    rsv39_handler,
    rsv40_handler,
    rsv41_handler,
    rsv42_handler,
    rsv43_handler,
    rsv44_handler,
    rsv45_handler,
    rsv46_handler,
    rsv47_handler,
    rsv48_handler,
    rsv49_handler,
    rsv50_handler,
    rsv51_handler,
    rsv52_handler,
    rsv53_handler,
    gpiowake_handler,
    rsv55_handler,
    spi3_handler,
    wdt1_handler,
    gpio3_handler,
    pt_handler,
    sdma_handler,
    hpb_handler,
    rsv62_handler,
    rsv63_handler,
    rsv64_handler,
    rsv65_handler,
    sdhc_handler,
    owm_handler,
    dma4_handler,
    dma5_handler,
    dma6_handler,
    dma7_handler,
    dma8_handler,
    dma9_handler,
    dma10_handler,
    dma11_handler,
    dma12_handler,
    dma13_handler,
    dma14_handler,
    dma15_handler,
    usbdma_handler
};

void init_system() {
    ChipRevision = MXC_SYS_GetRev();
    
    /* MAX3265x ROM turns off interrupts, which is not the same as the reset state. */
    __enable_irq();
    
    /* Enable FPU on Cortex-M4, which occupies coprocessor slots 10 & 11 */
    /* Grant full access, per "Table B3-24 CPACR bit assignments". */
    /* DDI0403D "ARMv7-M Architecture Reference Manual" */
    SCB->CPACR |= SCB_CPACR_CP10_Msk | SCB_CPACR_CP11_Msk;
    __DSB();
    __ISB();

    /* Change system clock source to the main high-speed clock */
    MXC_SYS_Clock_Select(MXC_SYS_CLOCK_HIRC96);
    SystemCoreClockUpdate();

    // Flush and enable instruction cache
    MXC_ICC->invalidate = 1;
    while (!(MXC_ICC->cache_ctrl & MXC_F_ICC_CACHE_CTRL_READY)) {}
    MXC_ICC->cache_ctrl |= MXC_F_ICC_CACHE_CTRL_ENABLE;
    while (!(MXC_ICC->cache_ctrl & MXC_F_ICC_CACHE_CTRL_READY)) {}

    /* Shutdown all peripheral clocks initially.  They will be re-enabled by each periph's init function. */
    /* GPIO Clocks are left enabled */
    MXC_SYS_ClockDisable(MXC_SYS_PERIPH_CLOCK_USB);
    MXC_SYS_ClockDisable(MXC_SYS_PERIPH_CLOCK_TFT);
    MXC_SYS_ClockDisable(MXC_SYS_PERIPH_CLOCK_DMA);
    MXC_SYS_ClockDisable(MXC_SYS_PERIPH_CLOCK_SPI0);
    MXC_SYS_ClockDisable(MXC_SYS_PERIPH_CLOCK_SPI1);
    MXC_SYS_ClockDisable(MXC_SYS_PERIPH_CLOCK_SPI2);
    MXC_SYS_ClockDisable(MXC_SYS_PERIPH_CLOCK_UART0);
    MXC_SYS_ClockDisable(MXC_SYS_PERIPH_CLOCK_UART1);
    MXC_SYS_ClockDisable(MXC_SYS_PERIPH_CLOCK_I2C0);
    MXC_SYS_ClockDisable(MXC_SYS_PERIPH_CLOCK_TPU);
    MXC_SYS_ClockDisable(MXC_SYS_PERIPH_CLOCK_TIMER0);
    MXC_SYS_ClockDisable(MXC_SYS_PERIPH_CLOCK_TIMER1);
    MXC_SYS_ClockDisable(MXC_SYS_PERIPH_CLOCK_TIMER2);
    MXC_SYS_ClockDisable(MXC_SYS_PERIPH_CLOCK_TIMER3);
    MXC_SYS_ClockDisable(MXC_SYS_PERIPH_CLOCK_TIMER4);
    MXC_SYS_ClockDisable(MXC_SYS_PERIPH_CLOCK_TIMER5);
    MXC_SYS_ClockDisable(MXC_SYS_PERIPH_CLOCK_ADC);
    MXC_SYS_ClockDisable(MXC_SYS_PERIPH_CLOCK_I2C1);
    MXC_SYS_ClockDisable(MXC_SYS_PERIPH_CLOCK_PT);
    MXC_SYS_ClockDisable(MXC_SYS_PERIPH_CLOCK_SPIXIPF);
    MXC_SYS_ClockDisable(MXC_SYS_PERIPH_CLOCK_SPIXIPM);
    MXC_SYS_ClockDisable(MXC_SYS_PERIPH_CLOCK_UART2);
    MXC_SYS_ClockDisable(MXC_SYS_PERIPH_CLOCK_TRNG);
    MXC_SYS_ClockDisable(MXC_SYS_PERIPH_CLOCK_FLC);
    MXC_SYS_ClockDisable(MXC_SYS_PERIPH_CLOCK_HBC);
    MXC_SYS_ClockDisable(MXC_SYS_PERIPH_CLOCK_SCACHE);
    MXC_SYS_ClockDisable(MXC_SYS_PERIPH_CLOCK_SDMA);
    MXC_SYS_ClockDisable(MXC_SYS_PERIPH_CLOCK_SEMA);
    MXC_SYS_ClockDisable(MXC_SYS_PERIPH_CLOCK_SDHC);
    MXC_SYS_ClockDisable(MXC_SYS_PERIPH_CLOCK_ICACHE);
    MXC_SYS_ClockDisable(MXC_SYS_PERIPH_CLOCK_ICACHEXIP);
    MXC_SYS_ClockDisable(MXC_SYS_PERIPH_CLOCK_OWIRE);
    MXC_SYS_ClockDisable(MXC_SYS_PERIPH_CLOCK_SPI3);
    MXC_SYS_ClockDisable(MXC_SYS_PERIPH_CLOCK_I2S);
    MXC_SYS_ClockDisable(MXC_SYS_PERIPH_CLOCK_SPIXIPR);
}
