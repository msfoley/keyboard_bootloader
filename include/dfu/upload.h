#ifndef DFU_UPLOAD_H
#define DFU_UPLOAD_H

#include <stdint.h>
#include <stdlib.h>

struct dfu_upload {
    uint32_t sequence_num;
    uint32_t total_len;
    uint32_t transmit_count;
};

struct dfu;

int dfu_upload_start(struct dfu *dfu, struct usb_setup_packet *packet);
int dfu_upload(struct dfu *dfu, struct usb_setup_packet *packet);

#endif
