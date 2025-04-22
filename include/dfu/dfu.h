#ifndef DFU_DFU_H
#define DFU_DFU_H

#include <stdint.h>

#include <usb/usb.h>
#include <usb/util.h>
#include <dfu/download.h>
#include <dfu/upload.h>

enum dfu_state {
    DFU_STATE_APP_IDLE,
    DFU_STATE_APP_DETACH,
    DFU_STATE_IDLE,
    DFU_STATE_DOWNLOAD_SYNC,
    DFU_STATE_DOWNLOAD_BUSY,
    DFU_STATE_DOWNLOAD_IDLE,
    DFU_STATE_MANIFEST_SYNC,
    DFU_STATE_MANIFEST,
    DFU_STATE_MANFIEST_WAIT_RESET,
    DFU_STATE_UPLOAD_IDLE,
    DFU_STATE_ERROR,
    DFU_STATE_DETACH,
    DFU_STATE_LEN
};

enum dfu_vendor_error {
    DFU_VENDOR_ERROR_LEN
};

enum dfu_request {
    DFU_REQUEST_DETACH,
    DFU_REQUEST_DOWNLOAD,
    DFU_REQUEST_UPLOAD,
    DFU_REQUEST_GET_STATUS,
    DFU_REQUEST_CLEAR_STATUS,
    DFU_REQUEST_GET_STATE,
    DFU_REQUEST_ABORT,
    DFU_REQUEST_LEN
};

enum dfu_status {
    DFU_STATUS_OK,
    DFU_STATUS_ERROR_TARGET,
    DFU_STATUS_ERROR_FILE,
    DFU_STATUS_ERROR_WRITE,
    DFU_STATUS_ERROR_ERASE,
    DFU_STATUS_ERROR_CHECK_ERASED,
    DFU_STATUS_ERROR_PROGRAM,
    DFU_STATUS_ERROR_VERIFY,
    DFU_STATUS_ERROR_ADDRESS,
    DFU_STATUS_ERROR_NOT_DONE,
    DFU_STATUS_ERROR_FIRMWARE,
    DFU_STATUS_ERROR_VENDOR,
    DFU_STATUS_ERROR_USB_RESET,
    DFU_STATUS_ERROR_POR,
    DFU_STATUS_ERROR_UNKNOWN,
    DFU_STATUS_ERROR_STALLED_PACKET,
    DFU_STATUS_LEN
};

struct dfu_download;

struct dfu {
    enum dfu_state state;
    enum dfu_status status;
    enum dfu_vendor_error verr;

    struct dfu_download download;
    struct dfu_upload upload;

    uint16_t desc_index_status;

    struct usb_request request;
    uint8_t control_data[64];

    int enumeration_complete;
    int dirty;
};

struct dfu_status_response {
    uint8_t status;
    uint8_t poll_timeout[3];
    uint8_t state;
    uint8_t string;
} __attribute__((packed));

struct dfu_state_response {
    uint8_t state;
} __attribute__((packed));

int dfu_read_control_data(void (*callback)(void *), struct dfu *dfu, uint8_t *data, uint16_t len);
int dfu_write_control_data(void (*callback)(void *), struct dfu *dfu, uint8_t *data, uint16_t len);

int dfu_init();
int dfu_poll_state();

#endif
