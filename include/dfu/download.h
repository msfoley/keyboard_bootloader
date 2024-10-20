#ifndef DFU_DOWNLOAD_H
#define DFU_DOWNLOAD_H

#include <stdint.h>

#include <usb.h>

#include <bootloader.h>

struct dfu_download {
    uint8_t **page;

    int current_page;
    int page_waiting_to_flush;

    size_t count;
    size_t page_count;
    int app_update; // false if a bootloader update

    struct bootloader_info image_info;
};

int dfu_download_init(struct dfu_download *dfu_download);
int dfu_download_check_done(struct dfu_download *state);
int dfu_download_flush_page(struct dfu_download *state);
void dfu_download_process(void *cbdata);

#endif
