#include <stdint.h>
#include <string.h>

#include <usb.h>
#include <enumerate.h>

#include <boot_usb/usb.h>
#include <dfu/dfu.h>
#include <dfu/download.h>

struct dfu dfu_state = {
    .state = DFU_STATE_IDLE,
};
struct dfu_download download_state;

int dfu_state_machine();

struct boot_usb_enum_callback dfu_class_req_callback = {
    .event = ENUM_CLASS_REQ,
    .callback = dfu_state_machine,
    .data = &dfu_state
};

int read_control_data(void (*callback)(void *), struct dfu *dfu, uint8_t *data, uint16_t len) {
    if (!dfu || !data) {
        return E_INVALID;
    }

    memset(&dfu->request, 0, sizeof(dfu->request));

    dfu->request.ep = 0;
    dfu->request.data = data;
    dfu->request.reqlen = len;
    dfu->request.callback = callback;
    dfu->request.cbdata = dfu;
    dfu->request.type = MAXUSB_TYPE_PKT;

    return MXC_USB_ReadEndpoint(&dfu->request);
}

int write_control_data(void (*callback)(void *), struct dfu *dfu, uint8_t *data, uint16_t len) {
    if (!dfu || !data) {
        return E_INVALID;
    }

    memset(&dfu->request, 0, sizeof(dfu->request));

    dfu->request.ep = 0;
    dfu->request.data = data;
    dfu->request.reqlen = len;
    dfu->request.callback = callback;
    dfu->request.cbdata = dfu;
    dfu->request.type = MAXUSB_TYPE_TRANS;

    return MXC_USB_WriteEndpoint(&dfu->request);
}

int dfu_send_status(struct dfu *dfu, uint32_t poll_timeout) {
    struct dfu_status_response *status;

    if (!dfu) {
        return E_INVALID;
    }

    status = (struct dfu_status_response *) dfu->control_data;

    if (poll_timeout & 0xFF000000) {
        poll_timeout = 0x00FFFFFF;
    }

    status->status = dfu->status;
    status->poll_timeout[0] = poll_timeout & 0x000000FF;
    status->poll_timeout[1] = (poll_timeout & 0x0000FF00) >> 8;
    status->poll_timeout[2] = (poll_timeout & 0x00FF0000) >> 16;
    status->state = dfu->state;
    status->string = dfu->status == DFU_STATUS_ERROR_VENDOR ? dfu->desc_index_status + dfu->verr : 0;

    return write_control_data(NULL, dfu, dfu->control_data, sizeof(status));
}

int dfu_send_state(struct dfu *dfu) {
    struct dfu_state_response *state;

    if (!dfu) {
        return E_INVALID;
    }

    state = (struct dfu_state_response *) dfu->control_data;

    state->state = dfu->state;

    return write_control_data(NULL, dfu, dfu->control_data, sizeof(state));
}

int dfu_init() {
    dfu_state.download = &download_state;

    return boot_usb_enum_register_callback(&dfu_class_req_callback);
}

int dfu_state_idle(MXC_USB_SetupPkt *req, struct dfu *dfu) {
    int ret = E_NO_ERROR;

    switch (req->bRequest) {
        case DFU_REQUEST_DOWNLOAD:
            if (req->wLength > 0) {
                // Handle incoming data
                dfu_download_init(dfu->download);
                dfu->state = DFU_STATE_DOWNLOAD_SYNC;

                read_control_data(dfu_download_process, dfu, dfu->control_data, req->wLength);
            } else {
                dfu->state = DFU_STATE_ERROR;
                dfu->status = DFU_STATUS_ERROR_STALLED_PACKET;
                MXC_USB_Stall(0);
            }
            break;
        case DFU_REQUEST_UPLOAD:
            if (req->wLength > 0) {
                // Handle outgoing data
                dfu->state = DFU_STATE_UPLOAD_IDLE;
            } else {
                dfu->state = DFU_STATE_ERROR;
                dfu->status = DFU_STATUS_ERROR_STALLED_PACKET;
                MXC_USB_Stall(0);
            }
            break;
        case DFU_REQUEST_ABORT:
            break;
        case DFU_REQUEST_GET_STATUS:
            dfu_send_status(dfu, 0);
            break;
        case DFU_REQUEST_GET_STATE:
            dfu_send_state(dfu);
            break;
        default:
            dfu->state = DFU_STATE_ERROR;
            dfu->status = DFU_STATUS_ERROR_STALLED_PACKET;
            MXC_USB_Stall(0);
            break;
    }

    return ret;
}

int dfu_state_download_sync(MXC_USB_SetupPkt *req, struct dfu *dfu) {
    int ret = 0;

    switch (req->bRequest) {
        case DFU_REQUEST_GET_STATUS:
            if (dfu->download->page_waiting_to_flush) {
                dfu_send_status(dfu, 500);
                dfu->state = DFU_STATE_DOWNLOAD_BUSY;
            } else {
                dfu_send_status(dfu, 0);
                dfu->state = DFU_STATE_DOWNLOAD_IDLE;
            }
            break;
        case DFU_REQUEST_GET_STATE:
            dfu_send_state(dfu);
            break;
        default:
            dfu->state = DFU_STATE_ERROR;
            dfu->status = DFU_STATUS_ERROR_STALLED_PACKET;
            MXC_USB_Stall(0);
            break;
    }

    return ret;
}

int dfu_state_download_busy(MXC_USB_SetupPkt *req, struct dfu *dfu) {
    int ret = 0;

    switch (req->bRequest) {
        default:
            dfu->state = DFU_STATE_ERROR;
            dfu->status = DFU_STATUS_ERROR_STALLED_PACKET;
            MXC_USB_Stall(0);
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
                read_control_data(dfu_download_process, dfu, dfu->control_data, req->wLength);

                dfu->state = DFU_STATE_DOWNLOAD_SYNC;
            } else {
                ret = dfu_download_check_done(dfu->download);
                // Image complete, flush
                if (!ret) {
                    dfu->state = DFU_STATE_MANIFEST_SYNC;
                } else {
                    // Uh-oh
                    dfu->state = DFU_STATE_ERROR;
                    dfu->status = ret;
                }
            }
            break;
        case DFU_REQUEST_ABORT:
            dfu->state = DFU_STATE_IDLE;
            break;
        case DFU_REQUEST_GET_STATUS:
            dfu_send_status(dfu, 0);
            break;
        case DFU_REQUEST_GET_STATE:
            dfu_send_state(dfu);
            break;
        default:
            dfu->state = DFU_STATE_ERROR;
            dfu->status = DFU_STATUS_ERROR_STALLED_PACKET;
            MXC_USB_Stall(0);
            break;
    }

    return ret;
}

int dfu_state_manifest_sync(MXC_USB_SetupPkt *req, struct dfu *dfu) {
    int ret = 0;

    switch (req->bRequest) {
        case DFU_REQUEST_GET_STATUS:
            if (dfu->download->page_waiting_to_flush) {
                dfu_send_status(dfu, 500);
                dfu->state = DFU_STATE_MANIFEST;
            } else {
                dfu_send_status(dfu, 0);
                dfu->state = DFU_STATE_IDLE;
            }
            break;
        case DFU_REQUEST_GET_STATE:
            dfu_send_state(dfu);
            break;
        default:
            dfu->state = DFU_STATE_ERROR;
            dfu->status = DFU_STATUS_ERROR_STALLED_PACKET;
            MXC_USB_Stall(0);
            break;
    }

    return ret;
}

int dfu_state_manifest(MXC_USB_SetupPkt *req, struct dfu *dfu) {
    int ret = 0;

    switch (req->bRequest) {
        default:
            dfu->state = DFU_STATE_ERROR;
            dfu->status = DFU_STATUS_ERROR_STALLED_PACKET;
            MXC_USB_Stall(0);
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
                if (0 /* If outgoing data < wLength, end upload */) {
                    dfu->state = DFU_STATE_IDLE;
                }
            }
            break;
        case DFU_REQUEST_ABORT:
            dfu->state = DFU_STATE_IDLE;
            break;
        case DFU_REQUEST_GET_STATUS:
            dfu_send_status(dfu, 0);
            break;
        case DFU_REQUEST_GET_STATE:
            dfu_send_state(dfu);
            break;
        default:
            dfu->state = DFU_STATE_ERROR;
            dfu->status = DFU_STATUS_ERROR_STALLED_PACKET;
            MXC_USB_Stall(0);
            break;
    }

    return ret;
}

int dfu_state_error(MXC_USB_SetupPkt *req, struct dfu *dfu) {
    int ret = 0;

    switch (req->bRequest) {
        case DFU_REQUEST_GET_STATUS:
            dfu_send_status(dfu, 0);
            break;
        case DFU_REQUEST_GET_STATE:
            dfu_send_state(dfu);
            break;
        case DFU_REQUEST_CLEAR_STATUS:
            dfu->status = DFU_STATUS_OK;
            dfu->state = DFU_STATE_IDLE;
            break;
        default:
            dfu->state = DFU_STATE_ERROR;
            dfu->status = DFU_STATUS_ERROR_STALLED_PACKET;
            MXC_USB_Stall(0);
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
