#include <stdint.h>

#include <boot_usb/descriptors.h>
#include <config.h>

__attribute__((aligned(4)))
struct usb_device_descriptor usb_device_descriptor = {
    .bLength = sizeof(device_descriptor),
    .bDescriptorType = 0x01, // Device descriptor
    .bcdUSB = 0x0200,
    .bDeviceClass = 0x00,
    .bDeviceSubClass = 0x00,
    .bDeviceProtocol = 0x00,
    .bMaxPacketSize = 64,
    .idVendor = USB_VENDOR_ID,
    .idProduct = USB_PRODUCT_ID,
    .bcdDevice = ((VERSION_MAJOR & 0xFF) << 8) | ((VERSION_MINOR) & 0xFF),
    .iManufacturer = STRING_DESCRIPTOR_MANUFACTURER,
    .iProduct = STRING_DESCRIPTOR_PRODUCT,
    .iSerialNumber = STRING_DESCRIPTOR_SERIAL_ID,
    .bNumConfigurations = 0x01
};

__attribute__((aligned(4)))
struct usb_product_config_descriptor usb_product_config_descriptor = {
    .config = {
        .bLength = sizeof(config_descriptor.config_descriptor),
        .bDescriptorType = 0x02, // Config descriptor type
        .wTotalLength = sizeof(config_descriptor),
        .bNumInterfaces = 0x01,
        .bConfigurationValue = 0x01,
        .iConfiguration = 0x00,
        .bmAttributes = 0x80, // Bus powered
        .bMaxPower = 0xFA // 500 mA (2 mA / bit)
    },
    .dfu_interface = {
        .bLength = sizeof(config_descriptor.interface_descriptor),
        .bDescriptorType = 0x04, // Interface descriptor type
        .bInterfaceNumber = 0x00,
        .bAlternateSetting = 0x00,
        .bNumEndpoints = 0x00,
        .bInterfaceClass = 0xFE, // DFU interface class
        .bInterfaceSubClass = 0x01,
        .bInterfaceProtocol = 0x02,
        .iInterface = STRING_DESCRIPTOR_DFU_INTERFACE
    },
    .dfu_functional = {
        .bLength = sizeof(config_descriptor.dfu_functional_descriptor),
        .bDescriptorType = 0x21,
        .bmAttributes = 0x0F,
        .wDetachTimeout = 60000, // Detach timeout 1s
        .wTransferSize = 64,
        .bcdDFUVersion = 0x0101
    },
};

const uint8_t string_descriptor_language[] = {
    4, 0x03, 0x09, 0x04
};

const uint8_t string_descriptor_manufacturer[] = {
    14, 0x03,
    'm', 0,
    'f', 0,
    'o', 0,
    'l', 0,
    'e', 0,
    'y', 0
};

const uint8_t string_descriptor_product[] = {
    18, 0x03,
    'k', 0,
    'e', 0,
    'y', 0,
    'b', 0,
    'o', 0,
    'a', 0,
    'r', 0,
    'd', 0
};

const uint8_t string_descriptor_serial[] = {
     20, 0x03,
     '0', 0,
     '0', 0,
     '0', 0,
     '0', 0,
     '0', 0,
     '0', 0,
     '0', 0,
     '0', 0,
     '1', 0
};

const uint8_t string_descriptor_dfu[] = {
    30, 0x03,
    'D', 0,
    'F', 0,
    'U', 0,
    ' ', 0,
    'B', 0,
    'o', 0,
    'o', 0,
    't', 0,
    'l', 0,
    'o', 0,
    'a', 0,
    'd', 0,
    'e', 0,
    'r', 0
};

struct usb_string_descriptor usb_string_descriptors[USB_STRING_DESCRIPTOR_LEN] = {
    [USB_STRING_DESCRIPTOR_LANGUAGE_ID] = {
        .data = string_descriptor_language,
    },
    [USB_STRING_DESCRIPTOR_MANUFACTURER] = {
        .data = string_descriptor_manufacturer,
    },
    [USB_STRING_DESCRIPTOR_PRODUCT] = {
        .data = string_descriptor_product,
    },
    [USB_STRING_DESCRIPTOR_SERIAL_ID] = {
        .data = string_descriptor_serial,
    },
    [USB_STRING_DESCRIPTOR_DFU_INTERFACE] = {
        .data = string_descriptor_dfu,
    },
}
