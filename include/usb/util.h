#ifndef USB_UTIL_H
#define USB_UTIL_H

#include <stdint.h>
#include <usb/device.h>

enum usb_request {
    USB_REQUEST_PACKET,
    USB_REQUEST_TRANSFER,
    USB_REQUEST_LEN
};

struct usb_request {
    uint32_t endpoint;
    uint8_t *data;
    uint32_t len;
    int status;

    void (*callback)(void *);
    void *data;

    enum usb_request type;
    uint8_t device_data[USB_REQUEST_DEVICE_DATA_LEN];
};

int usb_util_read_endpoint(struct usb_request *req);
int usb_util_write_endpoint(struct usb_request *req);
int usb_util_stall(uint32_t endpoint);
int usb_util_ack(uint32_t endpoint);

#endif
