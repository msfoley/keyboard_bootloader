#ifndef BOOT_USB_DESCRIPTORS_H
#define BOOT_USB_DESCRIPTORS_H

#include "usb.h"

enum string_descriptor {
    STRING_DESCRIPTOR_LANGUAGE_ID,
    STRING_DESCRIPTOR_MANUFACTURER,
    STRING_DESCRIPTOR_PRODUCT,
    STRING_DESCRIPTOR_SERIAL_ID,
    STRING_DESCRIPTOR_DFU_INTERFACE,
    STRING_DESCRIPTOR_LEN
};

struct boot_usb_dfu_functional_descriptor {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bmAttributes;
    uint16_t wDetachTimeout;
    uint16_t wTransferSize;
    uint16_t bcdDFUVersion;
} __attribute__((packed));

struct boot_usb_config_descriptor {
    MXC_USB_configuration_descriptor_t config_descriptor;
    MXC_USB_interface_descriptor_t interface_descriptor;
    struct boot_usb_dfu_functional_descriptor dfu_functional_descriptor;
} __attribute__((packed));

extern const MXC_USB_device_descriptor_t device_descriptor;
extern struct boot_usb_config_descriptor config_descriptor;

extern const uint8_t string_descriptor_language[];
extern const uint8_t string_descriptor_manufacturer[];
extern const uint8_t string_descriptor_product[];
extern const uint8_t string_descriptor_serial[];
extern const uint8_t string_descriptor_dfu[];

#endif
