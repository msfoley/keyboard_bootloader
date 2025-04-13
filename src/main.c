#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "mxc_device.h"
#include "mxc_sys.h"
#include "mxc_delay.h"
#include "nvic_table.h"
#include "uart.h"
#include "tmr.h"

#include <dfu/dfu.h>
#include <boot_usb/usb.h>
#include <bootloader.h>
#include <flash_control.h>
#include <flash_layout.h>

int main() {
    int ret;

    if (bootloader_shared.magic != BOOTLOADER_MAGIC) {
        MXC_UART_Init(MXC_UART_GET_UART(1), 115200);
        flash_control_init();
        boot_usb_init();
        dfu_init();

        boot_usb_start();

        do {
            ret = dfu_poll_state();
        } while (!ret);

        boot_usb_stop();

        jump(BOOTLOADER_START);
    }

    jump(APP_START);
}
