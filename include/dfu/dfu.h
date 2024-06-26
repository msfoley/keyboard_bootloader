#ifndef DFU_DFU_H
#define DFU_DFU_H

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
    DFU_STATE_LEN
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
    DFU_STATUS_LEN
};

struct dfu {
    enum dfu_state state;
    enum dfu_status status;
};

int dfu_init();

#endif
