#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <usb.h>

#include <util.h>
#include <dfu/dfu.h>
#include <dfu/download.h>
#include <bootloader.h>
#include <flash_control.h>

uint8_t page[2][PAGE_SIZE];

int dfu_download_init(struct dfu_download *dfu_download) {
    dfu_download->page = page;

    dfu_download->current_page = 0;
    dfu_download->page_waiting_to_flush = 0;

    dfu_download->count = 0;
    dfu_download->page_count = 0;
    dfu_download->app_update = 0;

    dfu_download->image_info.length = PAGE_SIZE;

    return 0;
}

int dfu_download_check_done(struct dfu_download *state) {
    struct dfu *dfu;

    if (!state) {
        return DFU_STATUS_ERROR_UNKNOWN;
    }
    dfu = container_of(state, struct dfu, download);

    if (state->count < state->image_info.length) {
        return DFU_STATUS_ERROR_NOT_DONE;
    }

    // Checksum

    if (state->page_count != 0) {
        memset(state->page[state->current_page] + state->page_count, 0xFF, PAGE_SIZE - state->page_count);
        state->current_page = !state->current_page;
        state->page_waiting_to_flush = 1;
    }

    return DFU_STATUS_OK;
}

int dfu_download_flush_page(struct dfu_download *state) {
    uint32_t offset;
    int ret;
    struct dfu *dfu;

    if (!state) {
        return DFU_STATUS_ERROR_UNKNOWN;
    }
    dfu = container_of(state, struct dfu, download);

    if (!state->page_waiting_to_flush) {
        return 0;
    }
    offset = state->app_update ? APP_START : BOOTLOADER_START;

    ret = flash_control_write_page(offset, state->page[!state->current_page]);
    if (ret) {
        dfu->state = DFU_STATE_ERROR;
        dfu->status = ret;

        goto err;
    }

    dfu->state = DFU_STATE_DOWNLOAD_SYNC;

err:
    state->page_waiting_to_flush = 0;
    return ret;
}

void dfu_download_process(void *cbdata) {
    MXC_USB_Req_t *req;
    struct dfu_download *state;
    struct dfu *dfu;
    uint8_t *page;
    uint32_t count;

    if (!cbdata) {
        return;
    }

    dfu = (struct dfu *) cbdata;
    req = &dfu->request;
    state = dfu->download;

    count = req->actlen;
    page = state->page[state->current_page];

    if ((state->page_count + req->actlen) >= PAGE_SIZE) {
        count = PAGE_SIZE - state->page_count;
        memcpy(page, req->data, count);

        state->page_count = 0;

        if (state->page_count < PAGE_SIZE) {
            memcpy(&state->image_info, state->page + BOOTLOADER_INFO_OFFSET, sizeof(state->image_info));

            if (state->image_info.magic != BOOTLOADER_MAGIC) {
                dfu->state = DFU_STATE_ERROR;
                dfu->state = DFU_STATUS_ERROR_TARGET;

                return;
            } else if (state->image_info.start_address == APP_START) {
                state->app_update = 1;
            } else if (state->image_info.start_address == BOOTLOADER_START) {
                state->app_update = 0;
            } else {
                dfu->state = DFU_STATE_ERROR;
                dfu->state = DFU_STATUS_ERROR_FILE;

                return;
            }
        } else {
            state->current_page = !state->current_page;
        }

        count = req->actlen - count;
        state->page_waiting_to_flush = 1;
    }

    memcpy(page, req->data, count);

    state->page_count = 0;
}
