#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include <usb.h>
#include <enumerate.h>

#include <boot_usb/usb.h>
#include <boot_usb/descriptors.h>
#include <dfu/dfu.h>
#include "strings.h"

struct dfu dfu_state = { 0 };

int dfu_state_machine(MXC_USB_SetupPkt *req, void *data);
int dfu_set_config(MXC_USB_SetupPkt *req, void *data);
int dfu_set_interface(MXC_USB_SetupPkt *req, void *data);

struct boot_usb_enum_callback dfu_class_req_callback = {
    .event = ENUM_CLASS_REQ,
    .callback = dfu_state_machine,
    .data = &dfu_state
};

struct boot_usb_enum_callback dfu_set_config_callback = {
    .event = ENUM_SETCONFIG,
    .callback = dfu_set_config,
    .data = &dfu_state
};

struct boot_usb_enum_callback dfu_set_interface_callback = {
    .event = ENUM_SETINTERFACE,
    .callback = dfu_set_interface,
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

void dfu_send_state_status_callback(void *data) {
    struct dfu *dfu = data;

    if (dfu->request.error_code) {
        dfu->state = DFU_STATE_ERROR;
        dfu->status = DFU_STATUS_ERROR_UNKNOWN;
        MXC_USB_Stall(0);
    } else {
        MXC_USB_Ackstat(0);
    }
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
    status->string = dfu->status + dfu->desc_index_status;

    return write_control_data(dfu_send_state_status_callback, dfu, dfu->control_data, sizeof(*status));
}

int dfu_send_state(struct dfu *dfu) {
    struct dfu_state_response *state;

    if (!dfu) {
        return E_INVALID;
    }

    state = (struct dfu_state_response *) dfu->control_data;

    state->state = dfu->state;

    return write_control_data(dfu_send_state_status_callback, dfu, dfu->control_data, sizeof(state));
}

int dfu_init() {
    dfu_state.state = DFU_STATE_IDLE;
    dfu_state.status = DFU_STATUS_OK;

    dfu_state.enumeration_complete = 0;
    dfu_state.dirty = 0;

    int ret = boot_usb_enum_register_callback(&dfu_class_req_callback);
    if (ret) {
        return ret;
    }

    ret = boot_usb_enum_register_callback(&dfu_set_config_callback);
    if (ret) {
        return ret;
    }

    ret = boot_usb_enum_register_callback(&dfu_set_interface_callback);
    if (ret) {
        return ret;
    }

    dfu_state.desc_index_status = boot_usb_register_string_descriptor((uint8_t **) dfu_status_msg, DFU_STATUS_LEN);

    return ret;
}

int dfu_state_idle(MXC_USB_SetupPkt *req, struct dfu *dfu) {
    int ret = E_NO_ERROR;

    switch (req->bRequest) {
        case DFU_REQUEST_DOWNLOAD:
            if (req->wLength > 0) {
                // Handle incoming data
                dfu_download_start(dfu);
                dfu->state = DFU_STATE_DOWNLOAD_SYNC;
                dfu->status = DFU_STATUS_OK;

                return read_control_data(dfu_download, dfu, dfu->control_data, req->wLength);
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
                dfu->status = DFU_STATUS_OK;
                MXC_USB_Ackstat(0);
            } else {
                dfu->state = DFU_STATE_ERROR;
                dfu->status = DFU_STATUS_ERROR_STALLED_PACKET;
                MXC_USB_Stall(0);
            }
            break;
        case DFU_REQUEST_ABORT:
            MXC_USB_Ackstat(0);
            break;
        case DFU_REQUEST_GET_STATUS:
            return dfu_send_status(dfu, 0);
        case DFU_REQUEST_GET_STATE:
            return dfu_send_state(dfu);
        case DFU_STATE_DETACH:
            if (dfu->dirty) {
                dfu->state = DFU_STATE_ERROR;
                dfu->status = DFU_STATUS_ERROR_FIRMWARE;
                MXC_USB_Stall(0);
            } else {
                dfu->state = DFU_STATE_DETACH;
                dfu->status = DFU_STATUS_OK;
                MXC_USB_Ackstat(0);
            }
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
            if (dfu->download.page_waiting_to_flush) {
                dfu->state = DFU_STATE_DOWNLOAD_BUSY;
                dfu->status = DFU_STATUS_OK;
                return dfu_send_status(dfu, 700);
            } else {
                dfu->state = DFU_STATE_DOWNLOAD_IDLE;
                dfu->status = DFU_STATUS_OK;
                return dfu_send_status(dfu, 0);
            }
            break;
        case DFU_REQUEST_GET_STATE:
            return dfu_send_state(dfu);
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
                read_control_data(dfu_download, dfu, dfu->control_data, req->wLength);

                dfu->state = DFU_STATE_DOWNLOAD_SYNC;
                dfu->status = DFU_STATUS_OK;
            } else {
                ret = dfu_download_check_done(dfu);
                // Image complete, flush
                if (!ret) {
                    dfu->state = DFU_STATE_MANIFEST_SYNC;
                    dfu->status = DFU_STATUS_OK;
                    MXC_USB_Ackstat(0);
                } else {
                    // Uh-oh
                    dfu->state = DFU_STATE_ERROR;
                    dfu->status = ret;
                    MXC_USB_Stall(0);
                }
            }
            break;
        case DFU_REQUEST_ABORT:
            dfu->state = DFU_STATE_IDLE;
            dfu->status = DFU_STATUS_OK;
            MXC_USB_Ackstat(0);
            break;
        case DFU_REQUEST_GET_STATUS:
            return dfu_send_status(dfu, 0);
        case DFU_REQUEST_GET_STATE:
            return dfu_send_state(dfu);
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
            if (!dfu->download.done) {
                dfu->state = DFU_STATE_MANIFEST;
                dfu->status = DFU_STATUS_OK;
                return dfu_send_status(dfu, 1200);
            } else {
                dfu->state = DFU_STATE_IDLE;
                dfu->status = DFU_STATUS_OK;
                return dfu_send_status(dfu, 0);
            }
            break;
        case DFU_REQUEST_GET_STATE:
            return dfu_send_state(dfu);
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
                    dfu->status = DFU_STATUS_OK;
                }
            }

            MXC_USB_Ackstat(0);
            break;
        case DFU_REQUEST_ABORT:
            dfu->state = DFU_STATE_IDLE;
            dfu->status = DFU_STATUS_OK;

            MXC_USB_Ackstat(0);
            break;
        case DFU_REQUEST_GET_STATUS:
            return dfu_send_status(dfu, 0);
        case DFU_REQUEST_GET_STATE:
            return dfu_send_state(dfu);
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
            return dfu_send_status(dfu, 0);
        case DFU_REQUEST_GET_STATE:
            return dfu_send_state(dfu);
        case DFU_REQUEST_CLEAR_STATUS:
            dfu->status = DFU_STATUS_OK;
            dfu->state = DFU_STATE_IDLE;

            MXC_USB_Ackstat(0);
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
            dfu_state_idle(req, data);
            break;
        case DFU_STATE_DOWNLOAD_SYNC:
            dfu_state_download_sync(req, data);
            break;
        case DFU_STATE_DOWNLOAD_BUSY:
            dfu_state_download_busy(req, data);
            break;
        case DFU_STATE_DOWNLOAD_IDLE:
            dfu_state_download_idle(req, data);
            break;
        case DFU_STATE_MANIFEST_SYNC:
            dfu_state_manifest_sync(req, data);
            break;
        case DFU_STATE_MANIFEST:
            dfu_state_manifest(req, data);
            break;
        case DFU_STATE_UPLOAD_IDLE:
            dfu_state_upload_idle(req, data);
            break;
        case DFU_STATE_ERROR:
            dfu_state_error(req, data);
            break;
        case DFU_STATE_MANFIEST_WAIT_RESET:
        default:
            MXC_USB_Stall(0);
            break;
    }

    return 2;
} 

int dfu_set_config(MXC_USB_SetupPkt *req, void *data) {
    struct dfu *dfu = data;

    if (req->wValue == config_descriptor.interface_descriptor.bInterfaceNumber) {
        dfu->enumeration_complete = 0x01;
    } else if (req->wValue == 0x0000) {
        dfu->enumeration_complete = 0x00;
    }
    
    return 0;
}

int dfu_set_interface(MXC_USB_SetupPkt *req, void *data) {
    return 0;
}

int dfu_poll_state() {
    struct dfu *dfu = &dfu_state;
    int ret = 0;

    switch (dfu->state) {
        case DFU_STATE_MANIFEST:
            dfu_download_manifest(dfu);
            break;
        case DFU_STATE_DOWNLOAD_BUSY:
            dfu_download_busy(dfu);
            break;
        case DFU_STATE_DETACH:
            ret = 1;
            break;
        default:
            break;
    }

    return ret;
}
