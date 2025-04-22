#ifndef DFU_DOWNLOAD_H
#define DFU_DOWNLOAD_H

#include <stdint.h>
#include <stdlib.h>

#include <bootloader.h>

struct dfu_download {
    uint8_t page[PAGE_SIZE];
    uint8_t first_page[PAGE_SIZE];
    uint8_t overflow[64];

    size_t current_page;
    int page_waiting_to_flush;

    size_t count;
    size_t page_count;
    size_t overflow_count;
    int app_update; // false if a bootloader update
    int done;

    struct bootloader_info image_info;
};

struct dfu;

int dfu_download_start(struct dfu *dfu);
int dfu_download_stop(struct dfu *dfu);
int dfu_download_check_done(struct dfu *dfu);
void dfu_download(void *cbdata);
void dfu_download_busy(struct dfu *dfu);
void dfu_download_manifest(struct dfu *dfu);

#endif
