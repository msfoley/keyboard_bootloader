#include <stdint.h>
#include <string.h>

#include <dfu/dfu.h>
#include <boot_usb/usb.h>
#include <bootloader.h>
#include <flash_control.h>
#include <flash_layout.h>
#include <util.h>

int main() {
    int ret;

    util_uart_init();
    util_printf("\033[2J");
    util_printf("Bootloader %02X.%02X\n", bootloader_info.version_major, bootloader_info.version_minor);

    if (bootloader_shared.magic != BOOTLOADER_MAGIC) {
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
