#ifndef USB_USB_H
#define USB_USB_H

#include <usb.h>
#include <usb_event.h>
#include <enumerate.h>

struct usb_setup_packet {
    uint8_t bmRequestType;
    uint8_t bRequest;
    uint16_t wValue;
    uint16_t wIndex;
    uint16_t wLength;
};

enum usb_event_type {
    USB_EVENT_BUS_RESET,
    USB_EVENT_VBUS,
    USB_EVENT_NO_VBUS,
    USB_EVENT_SETUP_DATA,
    USB_EVENT_LEN
};

enum usb_enum_request_type {
    USB_ENUM_REQUEST_CLASS,
    USB_ENUM_REQUEST_VENDOR,
    USB_ENUM_REQUEST_SET_CONFIG,
    USB_ENUM_REQUEST_SET_INTERFACE,
    USB_ENUM_REQUEST_GET_INTERFACE,
    USB_ENUM_REQUEST_SET_FEATURE,
    USB_ENUM_REQUEST_CLEAR_FEATURE,
    USB_ENUM_REQUEST_LEN
};

enum usb_descriptor_type {
    USB_DESCRIPTOR_DEVICE,
    USB_DESCRIPTOR_CONFIG,
    USB_DESCRIPTOR_OTHER,
    USB_DESCRIPTOR_QUAL,
    USB_DESCRIPTOR_STRING,
    USB_DESCRIPTOR_LEN
};

struct usb_callback {
    enum usb_event_type event;
    int (*callback)(enum usb_event_type event, void *data);
    void *data;

    struct usb_callback *next;
};

struct usb_enum_callback {
    enum usb_enum_request_type request;
    int (*callback)(struct usb_setup_packet *setup_packet, void *data);
    void *data;

    struct usb_enum_callback *next;
};

struct usb {
    struct usb_callback *callback_head[MAXUSB_NUM_EVENTS];
    struct usb_enum_callback *enum_callback_head[ENUM_NUM_CALLBACKS];

    uint8_t string_descriptor_index;
};

// Do not directly use outside of device specific USB code
extern struct usb usb;

int usb_init();
int usb_init_device();
int usb_start();
int usb_start_device();
int usb_stop();
int usb_stop_device();

int usb_register_callback(struct usb_callback *cb);
int usb_register_enum_callback(struct usb_enum_callback *cb);

int usb_register_string_descriptor(uint8_t **string_descriptor, uint8_t len);

int usb_register_descriptor(enum usb_descriptor_type, uint8_t index, uint8_t *data);

#endif
