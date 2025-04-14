#ifndef USB_DESCRIPTORS_H
#define USB_DESCRIPTORS_H

enum usb_string_descriptor_type {
    USB_STRING_DESCRIPTOR_LANGUAGE_ID,
    USB_STRING_DESCRIPTOR_MANUFACTURER,
    USB_STRING_DESCRIPTOR_PRODUCT,
    USB_STRING_DESCRIPTOR_SERIAL_ID,
    USB_STRING_DESCRIPTOR_DFU_INTERFACE,
    USB_STRING_DESCRIPTOR_LEN
};

struct usb_device_descriptor {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint16_t bcdUSB;
    uint8_t bDeviceClass;
    uint8_t bDeviceSubClass;
    uint8_t bDeviceProtocol;
    uint8_t bMaxPacketSize;
    uint16_t idVendor;
    uint16_t idProduct;
    uint16_t bcdDevice;
    uint8_t iManufacturer;
    uint8_t iProduct;
    uint8_t iSerialNumber;
    uint8_t bNumConfigurations;
} __attribute__((packed));

struct usb_config_descriptor {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint16_t wTotalLength;
    uint8_t bNumInterfaces;
    uint8_t bConfigurationValue;
    uint8_t iConfiguration;
    uint8_t bmAttributes;
    uint8_t bMaxPower;
} __attribute__((packed));

struct usb_interface_descriptor {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bInterfaceNumber;
    uint8_t bAlternateSetting;
    uint8_t bNumEndpoints;
    uint8_t bInterfaceClass;
    uint8_t bInterfaceSubClass;
    uint8_t bInterfaceProtocol;
    uint8_t iInterface;
} __attribute__((packed));

struct usb_dfu_functional_descriptor {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bmAttributes;
    uint16_t wDetachTimeout;
    uint16_t wTransferSize;
    uint16_t bcdDFUVersion;
} __attribute__((packed));

struct usb_endpoint_descriptor {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bEndpointAddress;
    uint8_t bmAttributes;
    uint16_t wMaxPacketSize;
    uint8_t bInterval;
} __attribute__((packed));

// Strings and their indices
struct usb_string_descriptor {
    const uint8_t *data;
};

// Weird name, but basically the packed storage of all config, interface, functional,
// endpoint, etc descriptors used in a product. Packed for the Maxim library, but other
// products may need different layouts. Cross that bridge when we get to it.
struct usb_product_config_descriptor {
    struct usb_config_descriptor config;
    struct usb_interface_descriptor dfu_interface;
    struct usb_dfu_functional_descriptor dfu_functional;
} __attribute__((packed));

extern struct usb_device_descriptor usb_device_descriptor;
extern struct usb_product_config_descriptor usb_product_config_descriptor;
extern struct usb_string_descriptor usb_string_descriptors[USB_STRING_DESCRIPTOR_LEN];

#endif
