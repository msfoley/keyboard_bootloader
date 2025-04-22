#include <stdint.h>
#include <string.h>

#include <dfu/dfu.h>
#include <usb/usb.h>
#include <bootloader.h>
#include <flash_control.h>
#include <memory_layout.h>
#include <util.h>

int main() {
    int ret;
    struct bootloader_info *app_info = (void *) (APP_START + BOOTLOADER_INFO_OFFSET);

    util_uart_init();
    util_printf("\033[2J");
    util_printf("Bootloader %02X.%02X\n", bootloader_info.version_major, bootloader_info.version_minor);

    uint32_t crc = calculate_image_crc32(BOOTLOADER_START);
    if (bootloader_info.checksum != crc) {
        util_printf("Bootloader CRC32 does not match.\n");
        util_printf("\tExpected:   0x%08X\n", bootloader_info.checksum);
        util_printf("\tCalculated: 0x%08X\n", crc);
    }

    int has_image = app_info->magic == BOOTLOADER_MAGIC;
    int checksum_valid = 0;
    if (has_image) {
        checksum_valid = calculate_image_crc32(APP_START) == app_info->checksum;
    }
    int magic_received = bootloader_shared.magic == BOOTLOADER_MAGIC;

    if (!has_image || !checksum_valid || magic_received) {
        util_printf("Entering DFU mode\n");
        if (magic_received) {
            util_printf("\tReceived bootloader magic flag\n");
        }

        if (!has_image) {
            util_printf("\tApp image does not exist\n");
        } else if (!checksum_valid) {
            util_printf("\tApp image checksum failed\n");
        }

        flash_control_init();
        usb_init();
        dfu_init();

        usb_start();

        do {
            ret = dfu_poll_state();
        } while (!ret);

        util_us_delay(500);

        usb_stop();

        jump(BOOTLOADER_START);
    }

    jump(APP_START);
}
