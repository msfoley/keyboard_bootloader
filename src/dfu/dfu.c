#include <stdint.h>

#include "usb.h"
#include "enumerate.h"

#include <boot_usb/usb.h>
#include <dfu/dfu.h>

struct dfu dfu_state = {
    .state = DFU_STATE_IDLE,
};

int dfu_state_machine();

struct boot_usb_enum_callback dfu_class_req_callback = {
    .event = ENUM_CLASS_REQ,
    .callback = dfu_state_machine,
    .data = &dfu_state
};

int dfu_init() {
    return boot_usb_enum_register_callback(&dfu_class_req_callback);
}

int dfu_state_idle(MXC_USB_SetupPkt *req, struct dfu *dfu) {
    int ret = E_NO_ERROR;

    switch (req->bRequest) {
        case DFU_REQUEST_DOWNLOAD:
            if (req->wLength > 0) {
                // Handle incoming data
                dfu->state = DFU_STATE_DOWNLOAD_SYNC;
            } else {
                // stall
            }
            break;
        case DFU_REQUEST_UPLOAD:
            if (req->wLength > 0) {
                // Handle outgoing data
                dfu->state = DFU_STATE_UPLOAD_IDLE;
            } else {
                // stall
            }
            break;
        case DFU_REQUEST_ABORT:
            break;
        case DFU_REQUEST_GET_STATUS:
            // send GET_STATUS
            break;
        case DFU_REQUEST_GET_STATE:
            // send GET_STATE
            break;
        default:
            // stall
            break;
    }

    return ret;
}

int dfu_state_machine(MXC_USB_SetupPkt *req, void *data) {
    struct dfu *dfu = data;

    switch (dfu_state.state) {
        case DFU_STATE_IDLE:
            break;
        case DFU_STATE_DOWNLOAD_SYNC:
            break;
        case DFU_STATE_DOWNLOAD_BUSY:
            break;
        case DFU_STATE_DOWNLOAD_IDLE:
            break;
        case DFU_STATE_MANIFEST_SYNC:
            break;
        case DFU_STATE_MANIFEST:
            break;
        case DFU_STATE_MANFIEST_WAIT_RESET:
            break;
        case DFU_STATE_UPLOAD_IDLE:
            break;
        case DFU_STATE_ERROR:
            break;
        default:
            break;
    }

    return E_NO_ERROR;
} 
