#include <stdint.h>
#include <string.h>
#include <errno.h>

#include <usb/usb.h>
#include <usb/util.h>
#include <usb/descriptors.h>
#include <dfu/dfu.h>

struct dfu dfu_state = { 0 };

int dfu_state_machine(struct usb_setup_packet *req, void *data);
int dfu_set_config(struct usb_setup_packet *req, void *data);
int dfu_set_interface(struct usb_setup_packet *req, void *data);

struct usb_enum_callback dfu_class_req_callback = {
    .request = USB_ENUM_REQUEST_CLASS,
    .callback = dfu_state_machine,
    .data = &dfu_state
};

struct usb_enum_callback dfu_set_config_callback = {
    .request = USB_ENUM_REQUEST_SET_CONFIG,
    .callback = dfu_set_config,
    .data = &dfu_state
};

struct usb_enum_callback dfu_set_interface_callback = {
    .request = USB_ENUM_REQUEST_SET_INTERFACE,
    .callback = dfu_set_interface,
    .data = &dfu_state
};

int dfu_read_control_data(void (*callback)(void *), struct dfu *dfu, uint8_t *data, uint16_t len) {
    if (!dfu || !data) {
        return EINVAL;
    }

    memset(&dfu->request, 0, sizeof(dfu->request));

    dfu->request.endpoint = 0;
    dfu->request.data = data;
    dfu->request.len = len;
    dfu->request.callback = callback;
    dfu->request.cbdata = dfu;
    dfu->request.type = USB_REQUEST_PACKET;

    return usb_util_read_endpoint(&dfu->request);
}

int dfu_write_control_data(void (*callback)(void *), struct dfu *dfu, uint8_t *data, uint16_t len) {
    if (!dfu || !data) {
        return EINVAL;
    }

    memset(&dfu->request, 0, sizeof(dfu->request));

    dfu->request.endpoint = 0;
    dfu->request.data = data;
    dfu->request.len = len;
    dfu->request.callback = callback;
    dfu->request.cbdata = dfu;
    dfu->request.type = USB_REQUEST_TRANSFER;

    return usb_util_write_endpoint(&dfu->request);
}

void dfu_send_state_status_callback(void *data) {
    struct dfu *dfu = data;

    if (dfu->request.status) {
        dfu->state = DFU_STATE_ERROR;
        dfu->status = DFU_STATUS_ERROR_UNKNOWN;
        usb_util_stall(0);
    } else {
        usb_util_ack(0);
    }
}

int dfu_send_status(struct dfu *dfu, uint32_t poll_timeout) {
    struct dfu_status_response *status;

    if (!dfu) {
        return EINVAL;
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
    status->string = 0;

    return dfu_write_control_data(dfu_send_state_status_callback, dfu, dfu->control_data, sizeof(*status));
}

int dfu_send_state(struct dfu *dfu) {
    struct dfu_state_response *state;

    if (!dfu) {
        return EINVAL;
    }

    state = (struct dfu_state_response *) dfu->control_data;

    state->state = dfu->state;

    return dfu_write_control_data(dfu_send_state_status_callback, dfu, dfu->control_data, sizeof(state));
}

int dfu_init() {
    dfu_state.state = DFU_STATE_IDLE;
    dfu_state.status = DFU_STATUS_OK;

    dfu_state.enumeration_complete = 0;
    dfu_state.dirty = 0;

    int ret = usb_register_enum_callback(&dfu_class_req_callback);
    if (ret) {
        return ret;
    }

    ret = usb_register_enum_callback(&dfu_set_config_callback);
    if (ret) {
        return ret;
    }

    ret = usb_register_enum_callback(&dfu_set_interface_callback);
    if (ret) {
        return ret;
    }

    return ret;
}

int dfu_state_idle(struct usb_setup_packet *req, struct dfu *dfu) {
    int ret = E_NO_ERROR;

    switch (req->bRequest) {
        case DFU_REQUEST_DOWNLOAD:
            if (req->wLength > 0) {
                // Handle incoming data
                dfu_download_start(dfu);
                dfu->state = DFU_STATE_DOWNLOAD_SYNC;
                dfu->status = DFU_STATUS_OK;

                return dfu_read_control_data(dfu_download, dfu, dfu->control_data, req->wLength);
            } else {
                dfu->state = DFU_STATE_ERROR;
                dfu->status = DFU_STATUS_ERROR_STALLED_PACKET;
                usb_util_stall(0);
            }
            break;
        case DFU_REQUEST_UPLOAD:
            if (req->wLength > 0) {
                // Handle outgoing data
                ret = dfu_upload_start(dfu, req);
                if (!ret) {
                    dfu->state = DFU_STATE_UPLOAD_IDLE;
                    dfu->status = DFU_STATUS_OK;
                } else {
                    dfu->state = DFU_STATE_ERROR;
                    dfu->status = ret;
                    usb_util_stall(0);
                }
            } else {
                dfu->state = DFU_STATE_ERROR;
                dfu->status = DFU_STATUS_ERROR_STALLED_PACKET;
                usb_util_stall(0);
            }
            break;
        case DFU_REQUEST_ABORT:
            usb_util_ack(0);
            break;
        case DFU_REQUEST_GET_STATUS:
            return dfu_send_status(dfu, 0);
        case DFU_REQUEST_GET_STATE:
            return dfu_send_state(dfu);
        case DFU_REQUEST_DETACH:
            if (dfu->dirty) {
                dfu->state = DFU_STATE_ERROR;
                dfu->status = DFU_STATUS_ERROR_FIRMWARE;
                usb_util_stall(0);
            } else {
                dfu->state = DFU_STATE_DETACH;
                dfu->status = DFU_STATUS_OK;
                usb_util_ack(0);
            }
            break;
        default:
            dfu->state = DFU_STATE_ERROR;
            dfu->status = DFU_STATUS_ERROR_STALLED_PACKET;
            usb_util_stall(0);
            break;
    }

    return ret;
}

int dfu_state_download_sync(struct usb_setup_packet *req, struct dfu *dfu) {
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
            usb_util_stall(0);
            break;
    }

    return ret;
}

int dfu_state_download_busy(struct usb_setup_packet *req, struct dfu *dfu) {
    int ret = 0;

    switch (req->bRequest) {
        default:
            dfu->state = DFU_STATE_ERROR;
            dfu->status = DFU_STATUS_ERROR_STALLED_PACKET;
            usb_util_stall(0);
            break;
    }

    return ret;
}

int dfu_state_download_idle(struct usb_setup_packet *req, struct dfu *dfu) {
    int ret = 0;

    switch (req->bRequest) {
        case DFU_REQUEST_DOWNLOAD:
            if (req->wLength > 0) {
                // Handle incoming data
                dfu_read_control_data(dfu_download, dfu, dfu->control_data, req->wLength);

                dfu->state = DFU_STATE_DOWNLOAD_SYNC;
                dfu->status = DFU_STATUS_OK;
            } else {
                ret = dfu_download_check_done(dfu);
                // Image complete, flush
                if (!ret) {
                    dfu->state = DFU_STATE_MANIFEST_SYNC;
                    dfu->status = DFU_STATUS_OK;
                } else {
                    // Uh-oh
                    dfu->state = DFU_STATE_ERROR;
                    dfu->status = ret;
                }
                usb_util_ack(0);
            }
            break;
        case DFU_REQUEST_ABORT:
            dfu->state = DFU_STATE_IDLE;
            dfu->status = DFU_STATUS_OK;
            usb_util_ack(0);
            break;
        case DFU_REQUEST_GET_STATUS:
            return dfu_send_status(dfu, 0);
        case DFU_REQUEST_GET_STATE:
            return dfu_send_state(dfu);
        default:
            dfu->state = DFU_STATE_ERROR;
            dfu->status = DFU_STATUS_ERROR_STALLED_PACKET;
            usb_util_stall(0);
            break;
    }

    return ret;
}

int dfu_state_manifest_sync(struct usb_setup_packet *req, struct dfu *dfu) {
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
            usb_util_stall(0);
            break;
    }

    return ret;
}

int dfu_state_manifest(struct usb_setup_packet *req, struct dfu *dfu) {
    int ret = 0;

    switch (req->bRequest) {
        default:
            dfu->state = DFU_STATE_ERROR;
            dfu->status = DFU_STATUS_ERROR_STALLED_PACKET;
            usb_util_stall(0);
            break;
    }

    return ret;
}

int dfu_state_upload_idle(struct usb_setup_packet *req, struct dfu *dfu) {
    int ret = 0;

    switch (req->bRequest) {
        case DFU_REQUEST_UPLOAD:
            if (req->wLength > 0) {
                // Handle outgoing data
                ret = dfu_upload(dfu, req);
                if (!ret) {
                    dfu->state = DFU_STATE_UPLOAD_IDLE;
                    dfu->status = DFU_STATUS_OK;
                } else {
                    dfu->state = DFU_STATE_ERROR;
                    dfu->status = ret;
                    usb_util_stall(0);
                }
            }
            break;
        case DFU_REQUEST_ABORT:
            dfu->state = DFU_STATE_IDLE;
            dfu->status = DFU_STATUS_OK;

            usb_util_ack(0);
            break;
        case DFU_REQUEST_GET_STATUS:
            return dfu_send_status(dfu, 0);
        case DFU_REQUEST_GET_STATE:
            return dfu_send_state(dfu);
        default:
            dfu->state = DFU_STATE_ERROR;
            dfu->status = DFU_STATUS_ERROR_STALLED_PACKET;
            usb_util_stall(0);
            break;
    }

    return ret;
}

int dfu_state_error(struct usb_setup_packet *req, struct dfu *dfu) {
    int ret = 0;

    switch (req->bRequest) {
        case DFU_REQUEST_GET_STATUS:
            return dfu_send_status(dfu, 0);
        case DFU_REQUEST_GET_STATE:
            return dfu_send_state(dfu);
        case DFU_REQUEST_CLEAR_STATUS:
            dfu->status = DFU_STATUS_OK;
            dfu->state = DFU_STATE_IDLE;

            usb_util_ack(0);
            break;
        default:
            dfu->state = DFU_STATE_ERROR;
            dfu->status = DFU_STATUS_ERROR_STALLED_PACKET;
            usb_util_stall(0);
            break;
    }

    return ret;
}

int dfu_state_machine(struct usb_setup_packet *req, void *data) {
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
            usb_util_stall(0);
            break;
    }

    return 2;
} 

int dfu_set_config(struct usb_setup_packet *req, void *data) {
    struct dfu *dfu = data;

    if (req->wValue == usb_product_config_descriptor.dfu_interface.bInterfaceNumber) {
        dfu->enumeration_complete = 0x01;
    } else if (req->wValue == 0x0000) {
        dfu->enumeration_complete = 0x00;
    }
    
    return 0;
}

int dfu_set_interface(struct usb_setup_packet *req, void *data) {
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
