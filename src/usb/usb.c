#include <stdint.h>
#include <stdlib.h>
#include <errno.h>

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
    int ret = 0;

    ret = usb_init_device();
    if (!ret) {
        int i;

        usb_register_descriptor(USB_DESCRIPTOR_DEVICE, 0, (uint8_t *) &usb_device_descriptor);
        usb_register_descriptor(USB_DESCRIPTOR_CONFIG, 0, (uint8_t *) &usb_product_config_descriptor);
        for (i = 0; i < USB_STRING_DESCRIPTOR_LEN; i++) {
            usb_register_descriptor(USB_DESCRIPTOR_STRING, i, usb_string_descriptors[i].data);
        }

        usb.string_descriptor_index = i;
    }

    return ret;
}

int usb_register_callback(struct usb_callback *cb) {
    struct usb_callback *head;

    if (cb == NULL || cb->event >= USB_EVENT_LEN) {
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

int usb_register_enum_callback(struct usb_enum_callback *cb) {
    struct usb_enum_callback *head;

    if (cb == NULL || cb->request >= USB_ENUM_REQUEST_LEN) {
        return 1;
    }

    if (usb.enum_callback_head[cb->request] == NULL) {
        usb.enum_callback_head[cb->request] = cb;
        cb->next = NULL;

        return 0;
    }

    head = usb.enum_callback_head[cb->request];
    while (head->next) {
        head = head->next;
    }

    head->next = cb;
    cb->next = NULL;

    return 0;
}

int usb_register_string_descriptor(const uint8_t **string_descriptor, uint8_t len) {
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
        usb_register_descriptor(USB_DESCRIPTOR_STRING, usb.string_descriptor_index++, string_descriptor[i]);
    }

    return index_start;
}
