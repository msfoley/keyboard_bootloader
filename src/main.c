#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "mxc_device.h"
#include "mxc_delay.h"
#include "nvic_table.h"
#include "uart.h"
#include "tmr.h"

#include <dfu/dfu.h>
#include <boot_usb/usb.h>
#include <bootloader.h>

int main() {
    if (bootloader_shared.magic != BOOTLOADER_MAGIC) {
        boot_usb_init();
        dfu_init();

        boot_usb_start();
    }

    while (1);
}
