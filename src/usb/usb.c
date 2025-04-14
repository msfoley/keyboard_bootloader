#include <stdint.h>
#include <stdlib.h>

#include "usb.h"
#include "usb_event.h"
#include "enumerate.h"
#include "mxc_delay.h"
#include "gcr_regs.h"
#include "mxc_sys.h"

#include <boot_usb/usb.h>
#include <boot_usb/descriptors.h>

struct boot_usb boot_usb = {
    .callback_head = { NULL },
    .enum_callback_head = { NULL },
};

void usb_handler(void) {
    MXC_USB_EventHandler();
}

void us_delay(unsigned int us) {
    uint32_t delay_cnt = (SystemCoreClock / 1000000) * us;

    while (delay_cnt--);
}

int startup_callback() {
    MXC_SYS_ClockEnable(MXC_SYS_PERIPH_CLOCK_USB);

    return E_NO_ERROR;
};

int shutdown_callback() {
    MXC_SYS_ClockDisable(MXC_SYS_PERIPH_CLOCK_USB);

    return E_NO_ERROR;
}

int boot_usb_stop() {
    MXC_USB_Shutdown();

    return E_NO_ERROR;
}

int boot_usb_event_callback(maxusb_event_t event, void *data) {
    int ret = 0;
    int err;

    switch (event) {
        case MAXUSB_EVENT_NOVBUS:
            MXC_USB_EventDisable(MAXUSB_EVENT_BRST);
            MXC_USB_Disconnect();
            enum_clearconfig();

            break;
        case MAXUSB_EVENT_VBUS:
            MXC_USB_EventClear(MAXUSB_EVENT_BRST);
            MXC_USB_EventEnable(MAXUSB_EVENT_BRST, boot_usb_event_callback, NULL);
            MXC_USB_Connect();

            break;
        case MAXUSB_EVENT_BRST:
            enum_clearconfig();

            break;
        default:
            break;
    }

    struct boot_usb_callback *head = boot_usb.callback_head[event];
    while (head != NULL) {
        err = head->callback(event, head->data);
        if (err != E_NO_ERROR) {
            ret = err;
        }

        head = head->next;
    }

    return ret;
}

int boot_usb_enum_callback(MXC_USB_SetupPkt *setup_packet, void *data) {
    struct boot_usb_enum_callback *head = *((struct boot_usb_enum_callback **) data);
    int ret = 0;
    int err;

    while (head != NULL) {
        err = head->callback(setup_packet, head->data);
        if (err != E_NO_ERROR) {
            ret = err;
        }

        head = head->next;
    }

    return ret;
}

int boot_usb_init() {
    int ret;
    maxusb_cfg_options_t usb_opts;

    usb_opts.enable_hs = 1;
    usb_opts.delay_us = us_delay;
    usb_opts.init_callback = startup_callback;
    usb_opts.shutdown_callback = shutdown_callback;

    ret = MXC_USB_Init(&usb_opts);
    if (ret != E_NO_ERROR) {
        return ret;
    }

    ret = enum_init();
    if (ret != E_NO_ERROR) {
        return ret;
    }

    enum_register_descriptor(ENUM_DESC_DEVICE, (uint8_t *)&device_descriptor, 0);
    enum_register_descriptor(ENUM_DESC_CONFIG, (uint8_t *)&config_descriptor, 0);
    enum_register_descriptor(ENUM_DESC_STRING, string_descriptor_language, STRING_DESCRIPTOR_LANGUAGE_ID);
    enum_register_descriptor(ENUM_DESC_STRING, string_descriptor_manufacturer, STRING_DESCRIPTOR_MANUFACTURER);
    enum_register_descriptor(ENUM_DESC_STRING, string_descriptor_product, STRING_DESCRIPTOR_PRODUCT);
    enum_register_descriptor(ENUM_DESC_STRING, string_descriptor_serial, STRING_DESCRIPTOR_SERIAL_ID);
    enum_register_descriptor(ENUM_DESC_STRING, string_descriptor_dfu, STRING_DESCRIPTOR_DFU_INTERFACE);
    boot_usb.string_descriptor_index = STRING_DESCRIPTOR_LEN;

    enum_register_callback(ENUM_CLASS_REQ, boot_usb_enum_callback, &boot_usb.enum_callback_head[ENUM_CLASS_REQ]); 
    enum_register_callback(ENUM_VENDOR_REQ, boot_usb_enum_callback, &boot_usb.enum_callback_head[ENUM_VENDOR_REQ]);
    enum_register_callback(ENUM_SETCONFIG, boot_usb_enum_callback, &boot_usb.enum_callback_head[ENUM_SETCONFIG]);
    enum_register_callback(ENUM_SETINTERFACE, boot_usb_enum_callback, &boot_usb.enum_callback_head[ENUM_SETINTERFACE]);
    enum_register_callback(ENUM_GETINTERFACE, boot_usb_enum_callback, &boot_usb.enum_callback_head[ENUM_GETINTERFACE]);
    enum_register_callback(ENUM_SETFEATURE, boot_usb_enum_callback, &boot_usb.enum_callback_head[ENUM_SETFEATURE]);
    enum_register_callback(ENUM_CLRFEATURE, boot_usb_enum_callback, &boot_usb.enum_callback_head[ENUM_CLRFEATURE]);

    MXC_USB_EventEnable(MAXUSB_EVENT_NOVBUS, boot_usb_event_callback, NULL);
    MXC_USB_EventEnable(MAXUSB_EVENT_VBUS, boot_usb_event_callback, NULL);

    return 0;
}

int boot_usb_start() {
    NVIC_EnableIRQ(USB_IRQn);

    return 0;
}

int boot_usb_register_callback(struct boot_usb_callback *cb) {
    struct boot_usb_callback *head;

    if (cb == NULL || cb->event >= MAXUSB_NUM_EVENTS) {
        return 1;
    }

    if (boot_usb.callback_head[cb->event] == NULL) {
        boot_usb.callback_head[cb->event] = cb;
        cb->next = NULL;

        return 0;
    }

    head = boot_usb.callback_head[cb->event];
    while (head->next) {
        head = head->next;
    }

    head->next = cb;
    cb->next = NULL;

    return 0;
}

int boot_usb_unregister_callback(struct boot_usb_callback *cb) {
    struct boot_usb_callback **head;

    if (cb == NULL || cb->event >= MAXUSB_NUM_EVENTS) {
        return 1;
    }

    if (boot_usb.callback_head[cb->event] == NULL) {
        return 0;
    }

    head = &boot_usb.callback_head[cb->event];
    while (*head) {
        if (*head == cb) {
            *head = (*head)->next;
        }

        head = &((*head)->next);
    }

    return 0;
}

int boot_usb_enum_register_callback(struct boot_usb_enum_callback *cb) {
    struct boot_usb_enum_callback *head;

    if (cb == NULL || cb->event >= ENUM_NUM_CALLBACKS) {
        return 1;
    }

    if (boot_usb.enum_callback_head[cb->event] == NULL) {
        boot_usb.enum_callback_head[cb->event] = cb;
        cb->next = NULL;

        return 0;
    }

    head = boot_usb.enum_callback_head[cb->event];
    while (head->next) {
        head = head->next;
    }

    head->next = cb;
    cb->next = NULL;

    return 0;
}

int boot_usb_enum_unregister_callback(struct boot_usb_enum_callback *cb) {
    struct boot_usb_enum_callback **head;

    if (cb == NULL || cb->event >= ENUM_NUM_CALLBACKS) {
        return 1;
    }

    if (boot_usb.enum_callback_head[cb->event] == NULL) {
        return 0;
    }

    head = &boot_usb.enum_callback_head[cb->event];
    while (*head) {
        if (*head == cb) {
            *head = (*head)->next;
        }

        head = &((*head)->next);
    }

    return 0;
}

int boot_usb_register_string_descriptor(uint8_t **string_descriptor, uint8_t len) {
    int index_start = boot_usb.string_descriptor_index;

    if (!string_descriptor) {
        return -E_INVALID;
    }

    for (int i = 0; i < len; i++) {
        if (!string_descriptor[i]) {
            return -E_INVALID;
        }
    }

    for (size_t i = 0; i < len; i++) {
        enum_register_descriptor(ENUM_DESC_STRING, string_descriptor[i], boot_usb.string_descriptor_index++);
    }

    return index_start;
}
