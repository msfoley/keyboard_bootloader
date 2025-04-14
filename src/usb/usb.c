#include <stdint.h>
#include <stdlib.h>

#include <util.h>
#include <usb/usb.h>
#include <usb/descriptors.h>

struct usb usb = {
    .callback_head = { NULL },
    .enum_callback_head = { NULL },
};

int usb_stop() {
    return usb_stop_device();
}

int usb_start() {
    return usb_start_device();
}

int usb_init() {
    int i;

    usb_register_descriptor(USB_DESCRIPTOR_DEVICE, (uint8_t *) &usb_device_descriptor, 0);
    usb_register_descriptor(USB_DESCRIPTOR_CONFIG, (uint8_t *) &usb_product_config_descriptor, 0);
    for (i = 0; i < USB_STRING_DESCRIPTOR_LEN; i++) {
        usb_register_descriptor(USB_DESCRIPTOR_STRING, usb_string_descriptors[i], i);
    }

    usb.string_descriptor_index = i;

    return usb_init_device();
}

int usb_register_callback(struct usb_callback *cb) {
    struct usb_callback *head;

    if (cb == NULL || cb->event >= MAXUSB_NUM_EVENTS) {
        return 1;
    }

    if (usb.callback_head[cb->event] == NULL) {
        usb.callback_head[cb->event] = cb;
        cb->next = NULL;

        return 0;
    }

    head = usb.callback_head[cb->event];
    while (head->next) {
        head = head->next;
    }

    head->next = cb;
    cb->next = NULL;

    return 0;
}

int usb_enum_register_callback(struct usb_enum_callback *cb) {
    struct usb_enum_callback *head;

    if (cb == NULL || cb->event >= ENUM_NUM_CALLBACKS) {
        return 1;
    }

    if (usb.enum_callback_head[cb->event] == NULL) {
        usb.enum_callback_head[cb->event] = cb;
        cb->next = NULL;

        return 0;
    }

    head = usb.enum_callback_head[cb->event];
    while (head->next) {
        head = head->next;
    }

    head->next = cb;
    cb->next = NULL;

    return 0;
}

int usb_register_string_descriptor(uint8_t **string_descriptor, uint8_t len) {
    int index_start = usb.string_descriptor_index;

    if (!string_descriptor) {
        return -EINVAL;
    }

    for (int i = 0; i < len; i++) {
        if (!string_descriptor[i]) {
            return -EINVAL;
        }
    }

    for (size_t i = 0; i < len; i++) {
        usb_register_descriptor(USB_DESCRIPTOR_STRING, string_descriptor[i], usb.string_descriptor_index++);
    }

    return index_start;
}
