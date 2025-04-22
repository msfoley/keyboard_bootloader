#include <stdlib.h>
#include <string.h>

#include <usb.h>
#include <usb/util.h>
#include <util.h>
#include <dfu/dfu.h>
#include <dfu/upload.h>
#include <bootloader.h>

int dfu_upload_start(struct dfu *dfu, struct usb_setup_packet *packet) {
    struct dfu_upload *state = &dfu->upload;
    struct bootloader_info *app_info = (void *) (APP_START + BOOTLOADER_INFO_OFFSET);
    int has_image = app_info->magic == BOOTLOADER_MAGIC;

    state->sequence_num = 0;
    state->transmit_count = 0;

    if (has_image) {
        state->total_len = BOOTLOADER_SIZE + app_info->length;
    } else {
        state->total_len = bootloader_info.length;
    }

    return dfu_upload(dfu, packet);
}

void dfu_upload_callback(void *data) {
    struct dfu *dfu = data;

    if (dfu->request.status) {
        dfu->state = DFU_STATE_ERROR;
        dfu->status = DFU_STATUS_ERROR_UNKNOWN;
        usb_util_stall(0);
    } else {
        usb_util_ack(0);
    }
}

int dfu_upload(struct dfu *dfu, struct usb_setup_packet *packet) {
    int len = packet->wLength;
    struct dfu_upload *state = &dfu->upload;
    uint8_t *flash = (uint8_t *) (BOOTLOADER_START + state->transmit_count);
    int ret;

    len = state->total_len < (state->transmit_count + len) ? (state->total_len - state->transmit_count) : len;

    state->transmit_count += len;
    state->sequence_num++;

    ret = dfu_write_control_data(dfu_upload_callback, dfu, flash, len);
    if (ret) {
        return DFU_STATUS_ERROR_UNKNOWN;
    }

    return 0;
}
