#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdio.h>

#include <usb.h>

#include <util.h>
#include <dfu/dfu.h>
#include <dfu/download.h>
#include <bootloader.h>
#include <flash_control.h>

#define MINIMUM_SIZE (BOOTLOADER_INFO_OFFSET + sizeof(state->image_info))

int dfu_download_start(struct dfu *dfu) {
    dfu->download.page_waiting_to_flush = 0;

    dfu->download.count = 0;
    dfu->download.page_count = 0;
    dfu->download.overflow_count = 0;
    dfu->download.app_update = 0;

    dfu->download.image_info.magic = 0;
    dfu->download.image_info.length = 0;

    dfu->download.done = 0;

    return 0;
}

int dfu_download_stop(struct dfu *dfu) {
    dfu->download.done = 1;

    return 0;
}

int dfu_download_check_done(struct dfu *dfu) {
    struct dfu_download *state = &dfu->download;

    if ((state->count + state->page_count) < state->image_info.length) {
        return DFU_STATUS_ERROR_NOT_DONE;
    } else if (state->count < MINIMUM_SIZE) {
        return DFU_STATUS_ERROR_TARGET;
    }

    // Checksum

    return DFU_STATUS_OK;
}

int dfu_download_verify_header(struct dfu_download *state) {
    memcpy(&state->image_info, state->page + BOOTLOADER_INFO_OFFSET, sizeof(state->image_info));

    if (state->image_info.magic != BOOTLOADER_MAGIC) {
        return DFU_STATUS_ERROR_TARGET;
    }

    switch (state->image_info.start_address) {
        case APP_START:
            state->app_update = 1;
            break;
        case BOOTLOADER_START:
            state->app_update = 0;
            break;
        default:
            return DFU_STATUS_ERROR_FILE;
    }

    return 0;
}

void dfu_download(void *cbdata) {
    uint32_t count;
    int ret;
    struct dfu *dfu = (struct dfu *) cbdata;
    struct dfu_download *state = &dfu->download;
    MXC_USB_Req_t *req = &dfu->request;
    uint32_t start_count = state->page_count;

    count = req->actlen > (PAGE_SIZE - state->page_count) ? PAGE_SIZE - state->page_count : req->actlen;
    state->overflow_count = req->actlen - count;

    memcpy(state->page + state->page_count, req->data, count);
    memcpy(state->overflow, req->data + count, state->overflow_count);

    state->page_count += count;
    state->page_waiting_to_flush = state->page_count == PAGE_SIZE;

    if ((state->count == 0) && (state->page_count >= MINIMUM_SIZE) && (start_count < MINIMUM_SIZE)) {
        ret = dfu_download_verify_header(state);
        if (ret) {
            dfu->state = DFU_STATE_ERROR;
            dfu->status = ret;

            MXC_USB_Ackstat(0);
            return;
        }
    }

    if (state->page_waiting_to_flush) {
        printf("Page %d complete - overflow %d\n", state->count / PAGE_SIZE, state->overflow_count);
    }

    MXC_USB_Ackstat(0);
}

void dfu_download_busy(struct dfu *dfu) {
    struct dfu_download *state = &dfu->download;
    uint32_t address = state->image_info.start_address + state->count;
    int ret;

    dfu->dirty = 1;
    ret = flash_control_erase_page(address);
    if (ret) {
        dfu->state = DFU_STATE_ERROR;
        dfu->status = ret;
        //MXC_USB_Stall(0);

        return;
    }

    if (state->count == 0) {
        // If this is the first page, save it off for later so we don't inadvertently boot bad code
        memcpy(state->first_page, state->page, PAGE_SIZE);
    } else {
        ret = flash_control_program_page(address, state->page);
        if (ret) {
            dfu->state = DFU_STATE_ERROR;
            dfu->status = ret;
            //MXC_USB_Stall(0);

            return;
        }
    }

    state->count += PAGE_SIZE;
    memcpy(state->page, state->overflow, state->overflow_count);
    state->page_count = state->overflow_count;
    state->overflow_count = 0;
    state->page_waiting_to_flush = 0;

    dfu->state = DFU_STATE_DOWNLOAD_SYNC;
    dfu->status = DFU_STATUS_OK;
}

void dfu_download_manifest(struct dfu *dfu) {
    struct dfu_download *state = &dfu->download;
    uint32_t address;
    int ret;

    if (state->page_count) {
        // Flush an in progress page
        address = state->image_info.start_address + state->count;
        // Don't program unnecessary bytes
        memset(state->page + state->page_count, 0xFF, PAGE_SIZE - state->page_count);

        ret = flash_control_erase_page(address);
        if (ret) {
            dfu->state = DFU_STATE_ERROR;
            dfu->status = ret;
            //MXC_USB_Stall(0);

            return;
        }

        ret = flash_control_program_page(address, state->page);
        if (ret) {
            dfu->state = DFU_STATE_ERROR;
            dfu->status = ret;
            //MXC_USB_Stall(0);

            return;
        }
    }

    ret = flash_control_program_page(state->image_info.start_address, state->first_page);
    if (ret) {
        dfu->state = DFU_STATE_ERROR;
        dfu->status = ret;

        return;
    }

    dfu->dirty = 0;

    dfu_download_stop(dfu);

    dfu->state = DFU_STATE_MANIFEST_SYNC;
    dfu->status = DFU_STATUS_OK;
}
