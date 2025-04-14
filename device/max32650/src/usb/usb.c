#include <stdint.h>
#include <stdlib.h>

#include <usb.h>
#include <usb_event.h>
#include <enumerate.h>
#include <mxc_delay.h>
#include <gcr_regs.h>
#include <mxc_sys.h>

#include <util.h>
#include <usb/usb.h>

int usb_event_callback(maxusb_event_t event, void *data);
int usb_enum_callback(MXC_USB_SetupPkt *setup_packet, void *data);

enum usb_enum_request_type request_type_lookup[ENUM_NUM_CALLBACKS] = {
    [ENUM_CLASS_REQ] = USB_ENUM_REQUEST_CLASS,
    [ENUM_VENDOR_REQ] = USB_ENUM_REQUEST_VENDOR,
    [ENUM_SETCONFIG] = USB_ENUM_REQUEST_SET_CONFIG,
    [ENUM_SETINTERFACE] = USB_ENUM_REQUEST_SET_INTERFACE,
    [ENUM_GETINTERFACE] = USB_ENUM_REQUEST_GET_INTERFACE,
    [ENUM_SETFEATURE] = USB_ENUM_REQUEST_SET_FEATURE,
    [ENUM_CLRFEATURE] = USB_ENUM_REQUEST_CLEAR_FEATURE
};

enum_callback_t device_request_type_lookup[USB_ENUM_REQUEST_LEN] = {
    [USB_ENUM_REQUEST_CLASS] = ENUM_CLASS_REQ,
    [USB_ENUM_REQUEST_VENDOR] = ENUM_VENDOR_REQ,
    [USB_ENUM_REQUEST_SET_CONFIG] = ENUM_SETCONFIG,
    [USB_ENUM_REQUEST_SET_INTERFACE] = ENUM_SETINTERFACE,
    [USB_ENUM_REQUEST_GET_INTERFACE] = ENUM_GETINTERFACE,
    [USB_ENUM_REQUEST_SET_FEATURE] = ENUM_SETFEATURE,
    [USB_ENUM_REQUEST_CLEAR_FEATURE] = ENUM_CLRFEATURE
};

enum usb_event_type event_type_lookup[MAXUSB_NUM_EVENTS] = {
    [MAXUSB_EVENT_DPACT] = USB_EVENT_LEN,
    [MAXUSB_EVENT_RWUDN] = USB_EVENT_LEN,
    [MAXUSB_EVENT_BACT] = USB_EVENT_LEN,
    [MAXUSB_EVENT_BRST] = USB_EVENT_BUS_RESET,
    [MAXUSB_EVENT_SUSP] = USB_EVENT_LEN,
    [MAXUSB_EVENT_NOVBUS] = USB_EVENT_NO_VBUS,
    [MAXUSB_EVENT_VBUS] = USB_EVENT_VBUS,
    [MAXUSB_EVENT_BRSTDN] = USB_EVENT_LEN,
    [MAXUSB_EVENT_SUDAV] = USB_EVENT_SETUP_DATA
};

maxusb_event_t device_event_type_lookup[USB_EVENT_LEN] = {
    [USB_EVENT_BUS_RESET] = MAXUSB_EVENT_BRST,
    [USB_EVENT_NO_VBUS] = MAXUSB_EVENT_NOVBUS,
    [USB_EVENT_VBUS] = MAXUSB_EVENT_VBUS,
    [USB_EVENT_SETUP_DATA] = MAXUSB_EVENT_SUDAV
};

enum usb_descriptor_type descriptor_type_lookup[USB_DESCRIPTOR_LEN] = {
    [ENUM_DESC_DEVICE] = USB_DESCRIPTOR_DEVICE,
    [ENUM_DESC_CONFIG] = USB_DESCRIPTOR_CONFIG,
    [ENUM_DESC_OTHER] = USB_DESCRIPTOR_OTHER,
    [ENUM_DESC_QUAL] = USB_DESCRIPTOR_QUAL,
    [ENUM_DESC_STRING] = USB_DESCRIPTOR_STRING
};

enum_descriptor_t device_descriptor_type_lookup[USB_DESCRIPTOR_LEN] = {
    [USB_DESCRIPTOR_DEVICE] = ENUM_DESC_DEVICE,
    [USB_DESCRIPTOR_CONFIG] = ENUM_DESC_CONFIG,
    [USB_DESCRIPTOR_OTHER] = ENUM_DESC_OTHER,
    [USB_DESCRIPTOR_QUAL] = ENUM_DESC_QUAL,
    [USB_DESCRIPTOR_STRING] = ENUM_DESC_STRING
};

void usb_handler(void) {
    MXC_USB_EventHandler();
}

int startup_callback() {
    MXC_SYS_ClockEnable(MXC_SYS_PERIPH_CLOCK_USB);

    return E_NO_ERROR;
};

int shutdown_callback() {
    MXC_SYS_ClockDisable(MXC_SYS_PERIPH_CLOCK_USB);

    return E_NO_ERROR;
}

int usb_start() {
    NVIC_EnableIRQ(USB_IRQn);

    return 0;
}

int usb_stop_device() {
    MXC_USB_Disconnect();
    MXC_USB_Shutdown();
    NVIC_DisableIRQ(USB_IRQn);

    return E_NO_ERROR;
}

int usb_init_device() {
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

    for (int i = 0; i < USB_ENUM_REQUEST_LEN; i++) {
        enum_register_callback(device_request_type_lookup[i], usb_enum_callback, &usb.enum_callback_head[i]);
    }

    MXC_USB_EventEnable(MAXUSB_EVENT_NOVBUS, usb_event_callback, NULL);
    MXC_USB_EventEnable(MAXUSB_EVENT_VBUS, usb_event_callback, NULL);

    return 0;
}

int usb_register_descriptor(enum usb_descriptor_type type, uint8_t index, uint8_t *data) {
    enum_descriptor_t device_type;

    return enum_register_callback(device_descriptor_lookup[device_type], data, index);
}

int usb_event_callback(maxusb_event_t event, void *data) {
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
            MXC_USB_EventEnable(MAXUSB_EVENT_BRST, usb_event_callback, NULL);
            MXC_USB_Connect();

            break;
        case MAXUSB_EVENT_BRST:
            enum_clearconfig();

            break;
        default:
            break;
    }

    if (event_type_lookup[event] != USB_EVENT_LEN) {
        struct usb_callback *head = usb.callback_head[event];
        while (head != NULL) {
            err = head->callback(event_type_lookup[event], head->data);
            if (err != E_NO_ERROR) {
                ret = err;
            }

            head = head->next;
        }
    }

    return ret;
}

int usb_enum_callback(MXC_USB_SetupPkt *setup_packet, void *data) {
    struct usb_enum_callback *head = *((struct usb_enum_callback **) data);
    int ret = 0;
    int err;

    while (head != NULL) {
        head->callback(setup_packet, head->data);

        head = head->next;
    }

    // Individual subsystems will be responsible acking and naking as necessary
    return 2;
}
