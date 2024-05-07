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

int dfu_state_download_sync(MXC_USB_SetupPkt *req, struct dfu *dfu) {
    int ret;

    switch (req->bRequest) {
        case DFU_REQUEST_GET_STATUS:
            if (false /* If we need to flush the page buffer */) {
                dfu->state = DFU_STATE_DOWNLOAD_BUSY;
            } else {
                dfu->state = DFU_STATE_DOWNLOAD_IDLE;
            }
            // Send GET_STATUS
            break;
        case DFU_REQUEST_GET_STATE:
            // Send GET_STATE
            break;
        default:
            // stall
    }

    return ret;
}

int dfu_state_download_busy(MXC_USB_SetupPkt *req, struct dfu *dfu) {
    int ret = 0;

    switch (req->bRequest) {
        default:
            // Stall
            break;
    }

    return ret;
}

int dfu_state_download_idle(MXC_USB_SetupPkt *req, struct dfu *dfu) {
    int ret = 0;

    switch (req->bRequest) {
        case DFU_REQUEST_DOWNLOAD:
            if (req->wLength > 0) {
                // Handle incoming data
            } else {
                // Image complete, flush
                if (false /* We agree image is done */) {
                } else {
                    // Uh-oh
                    dfu->state = DFU_STATE_ERROR;
                }
            }
            break;
        case DFU_REQUEST_ABORT:
            dfu->state = DFU_STATE_IDLE;
            break;
        case DFU_REQUEST_GET_STATUS:
            // Send GET_STATUS
            break;
        case DFU_REQUEST_GET_STATE:
            // Send GET_STATE
            break;
        default:
            // Stall
            break;
    }

    return ret;
}

int dfu_state_manifest_sync(MXC_USB_SetupPkt *req, struct dfu *dfu) {
    int ret = 0;

    switch (req->bRequest) {
        case DFU_REQUEST_GET_STATUS:
            if (false /* still flushing/"manifesting" */) {
            } else {
                dfu->state = DFU_STATE_IDLE;
            }
            // Send GET_STATUS
            break;
        case DFU_REQUEST_GET_STATE:
            // Send GET_STATE
            break;
        default:
            // Stall
            break;
    }

    return ret;
}

int dfu_state_manifest(MXC_USB_SetupPkt *req, struct dfu *dfu) {
    int ret = 0;

    switch (req->bRequest) {
        default:
            // Stall
            break;
    }

    return ret;
}

int dfu_state_upload_idle(MXC_USB_SetupPkt *req, struct dfu *dfu) {
    int ret = 0;

    switch (req->bRequest) {
        case DFU_REQUEST_UPLOAD:
            if (req->wLength > 0) {
                // Handle outgoing data
                if (false /* If outgoing data < wLength, end upload */) {
                    dfu->state = DFU_STATE_IDLE
                }
            }
            break;
        case DFU_REQUEST_ABORT:
            dfu->state = DFU_STATE_ABORT;
            break;
        case DFU_REQUEST_GET_STATUS:
            // Send GET_STATUS
            break;
        case DFU_REQUEST_GET_STATE:
            // Send GET_STATE
            break;
        default:
            // Stall
            break;
    }

    return ret;
}

int dfu_state_error(MXC_USB_SetupPkt *req, struct dfu *dfu) {
    int ret = 0;

    switch (req->bRequest) {
        case DFU_REQUEST_GET_STATUS:
            // Send GET_STATUS
            break;
        case DFU_REQUEST_GET_STATE:
            // Send GET_STATE
            break;
        case DFU_REQUEST_CLEAR_STATUS:
            dfu->status = DFU_STATUS_OK;
            dfu->state = DFU_STATE_IDLE;
            break;
        default:
            // Stall
            break;
    }

    return ret;
}

int dfu_state_machine(MXC_USB_SetupPkt *req, void *data) {
    struct dfu *dfu = data;

    switch (dfu->state) {
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
