#include <stdint.h>
#include <stdlib.h>

#include <usb.h>
#include <usb_event.h>
#include <enumerate.h>

#include <util.h>
#include <usb/usb.h>
#include <usb/util.h>

void usb_util_endpoint_callback(void *data) {
    struct usb_request *req = data;
    MXC_USB_Req_t *mxc_req = (MXC_USB_Req_t *) req->device_data;

    req->len = mxc_req->actlen;
    req->status = mxc_req->error_code;

    if (req->callback) {
        req->callback(req->data);
    }
}

int usb_util_read_endpoint(struct usb_request *req) {
    MXC_USB_Req_t *mxc_req = (MXC_USB_Req_t *) req->device_data;

    mxc_req->ep = req->endpoint;
    mxc_req->data = req->data;
    mxc_req->reqlen = req->len;
    mxc_req->callback = usb_util_endpoint_callback;
    mxc_req->cbdata = req;
    mxc_req->type = req->type == USB_REQUEST_PACKET ? MAXUSB_TYPE_PKT : MAXUSB_TYPE_TRANS;

    return MXC_USB_ReadEndpoint(mxc_req);
}

int usb_util_write_endpoint(struct usb_request *req) {
    MXC_USB_Req_t *mxc_req = (MXC_USB_Req_t *) req->device_data;
    
    mxc_req->ep = req->endpoint;
    mxc_req->data = req->data;
    mxc_req->reqlen = req->len;
    mxc_req->callback = usb_util_endpoint_callback;
    mxc_req->cbdata = req;
    mxc_req->type = req->type == USB_REQUEST_PACKET ? MAXUSB_TYPE_PKT : MAXUSB_TYPE_TRANS;

    return MXC_USB_WriteEndpoint(mxc_req);
}

int usb_util_stall(uint32_t endpoint) {
    return MXC_USB_Stall(endpoint);
}

int usb_util_ack(uint32_t endpoint) {
    return MXC_USB_Ackstat(endpoint);
}
