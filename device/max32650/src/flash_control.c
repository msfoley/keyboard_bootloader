#include <stdint.h>
#include <errno.h>

#include <flc_regs.h>
#include <mxc_sys.h>
#include <flc.h>

#include <dfu/dfu.h>
#include <device.h>
#include <flash_control.h>
#include <flash_layout.h>
#include <util.h>

int flash_control_init() {
    MXC_FLC_Init();

    return 0;
}

int flash_control_erase_page(uint32_t address) {
    int ret;

    util_printf("Erasing 0x%08X\n", address);

    address = address & ~(PAGE_SIZE - 1);

    ret = MXC_FLC_PageErase(address);
    if (ret) {
        return DFU_STATUS_ERROR_ERASE;
    }

    for (uint32_t i = 0; i < PAGE_SIZE; i += 4) {
        uint32_t written_value = 0xFFFFFFFF;
        uint32_t read_value = *((uint32_t *) (address + i));

        if (written_value != read_value) {
            util_printf("Erase check failed at 0x%08X: 0x%08X != 0x%08X\n", address + i, written_value, read_value);
            return DFU_STATUS_ERROR_CHECK_ERASED;
        }
    }

    return 0;
}

int flash_control_program_page(uint32_t address, uint8_t *page) {
    int ret;

    util_printf("Programming 0x%08X\n", address);
    address = address & ~(PAGE_SIZE - 1);

    for (uint32_t i = 0; i < PAGE_SIZE; i += 16) {
        ret = MXC_FLC_Write128(address + i, (uint32_t *) (page + i));
        if (ret) {
            return DFU_STATUS_ERROR_PROGRAM;
        }
    }

    for (uint32_t i = 0; i < PAGE_SIZE; i += 4) {
        uint32_t written_value = *((uint32_t *) (page + i));
        uint32_t read_value = *((uint32_t *) (address + i));

        if (written_value != read_value) {
            util_printf("Program check failed at 0x%08X: 0x%08X != 0x%08X\n", address + i, written_value, read_value);
            return DFU_STATUS_ERROR_VERIFY;
        }
    }

    return 0;
}
