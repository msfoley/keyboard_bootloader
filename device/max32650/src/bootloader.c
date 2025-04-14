#include <stdint.h>

#include <gcr_regs.h>

#include <device.h>
#include <bootloader.h>
#include <config.h>

void jump_device(uint32_t address) {
    MXC_GCR->rst0 |= MXC_F_GCR_RST0_PERIPH | MXC_F_GCR_RST0_SOFT;
}
