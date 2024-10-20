#include <stdint.h>
#include <errno.h>

#include <flc_regs.h>

#include <dfu/dfu.h>
#include <device.h>
#include <flash_control.h>
#include <flash_layout.h>

int flash_control_write_page(uint32_t address, uint8_t *page) {
    if (!page) {
        return DFU_STATUS_ERROR_UNKNOWN;
    }

    if (address & ~0x1FF000) {
        return DFU_STATUS_ERROR_ADDRESS;
    }

    MXC_FLC->ctrl &= ~MXC_F_FLC_CTRL_WIDTH;
    MXC_FLC->ctrl |= MXC_S_FLC_CTRL_UNLOCK_CODE_UNLOCKED;
    while (MXC_FLC->ctrl & MXC_F_FLC_CTRL_BUSY);

    MXC_FLC->addr = 0x10000000 | address;

    MXC_FLC->ctrl |= MXC_S_FLC_CTRL_ERASE_CODE_PGE;
    MXC_FLC->ctrl |= MXC_S_FLC_CTRL_PAGE_ERASE_START_PGE;
    while (!(MXC_FLC->intr & MXC_F_FLC_INTR_DONE));
    while (MXC_FLC->ctrl & MXC_F_FLC_CTRL_BUSY);

    if (MXC_FLC->intr & MXC_F_FLC_INTR_ACCESS_FAIL) {
        return DFU_STATUS_ERROR_ERASE;
    }

    for (uint32_t i = 0; i < PAGE_SIZE; i += 4) {
        if (*((uint32_t *) (address + i)) != 0xFFFFFFFF) {
            return DFU_STATUS_ERROR_CHECK_ERASED;
        }
    }

    for (uint32_t i = 0; i < PAGE_SIZE; i += 16) {
        MXC_FLC->addr = 0x10000000 | (address + i);

        MXC_FLC->data[0] = ((uint32_t *) (page + i))[0];
        MXC_FLC->data[1] = ((uint32_t *) (page + i))[1];
        MXC_FLC->data[2] = ((uint32_t *) (page + i))[2];
        MXC_FLC->data[3] = ((uint32_t *) (page + i))[3];

        MXC_FLC->ctrl |= MXC_F_FLC_CTRL_WRITE;

        while (!(MXC_FLC->intr & MXC_F_FLC_INTR_DONE));
        while (MXC_FLC->ctrl & MXC_F_FLC_CTRL_BUSY);

        if (MXC_FLC->intr & MXC_F_FLC_INTR_ACCESS_FAIL) {
            return DFU_STATUS_ERROR_PROGRAM;
        }
    }

    for (uint32_t i = 0; i < PAGE_SIZE; i += 4) {
        if (*((uint32_t *) (address + i)) != *((uint32_t *) (page + i))) {
            return DFU_STATUS_ERROR_VERIFY;
        }
    }

    return 0;
}
