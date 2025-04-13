#ifndef BOOT_USB_USB_H
#define BOOT_USB_USB_H

#include <usb.h>
#include <usb_event.h>
#include <enumerate.h>

struct boot_usb_callback {
    maxusb_event_t event;
    int (*callback)(maxusb_event_t event, void *data);
    void *data;

    struct boot_usb_callback *next;
};

struct boot_usb_enum_callback {
    enum_callback_t event;
    int (*callback)(MXC_USB_SetupPkt *setup_packet, void *data);
    void *data;

    struct boot_usb_enum_callback *next;
};

struct boot_usb {
    struct boot_usb_callback *callback_head[MAXUSB_NUM_EVENTS];
    struct boot_usb_enum_callback *enum_callback_head[ENUM_NUM_CALLBACKS];

    uint8_t string_descriptor_index;
};

int boot_usb_init();
int boot_usb_start();
int boot_usb_stop();

int boot_usb_register_callback(struct boot_usb_callback *cb);
int boot_usb_unregister_callback(struct boot_usb_callback *cb);
int boot_usb_enum_register_callback(struct boot_usb_enum_callback *cb);
int boot_usb_enum_unregister_callback(struct boot_usb_enum_callback *cb);

int boot_usb_register_string_descriptor(uint8_t **string_descriptor, uint8_t len);

#endif
