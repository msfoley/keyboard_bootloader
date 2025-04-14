#include <stdint.h>
#include <string.h>

#include <mxc_device.h>
#include <mxc_sys.h>
#include <mxc_delay.h>
#include <nvic_table.h>
#include <uart.h>
#include <tmr.h>

#include <dfu/dfu.h>
#include <boot_usb/usb.h>
#include <bootloader.h>
#include <flash_control.h>
#include <flash_layout.h>
#include <util.h>

int main() {
    int ret;

    boot_uart_init();
    boot_printf("\033[2J");
    boot_printf("Bootloader %02X.%02X\n", bootloader_info.version_major, bootloader_info.version_minor);

    if (bootloader_shared.magic != BOOTLOADER_MAGIC) {
        flash_control_init();
        boot_usb_init();
        dfu_init();

        boot_usb_start();

        do {
            ret = dfu_poll_state();
        } while (!ret);

        boot_usb_stop();

        us_delay(500);

        jump(BOOTLOADER_START);
    }

    jump(APP_START);
}
