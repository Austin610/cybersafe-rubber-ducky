#include "tusb.h"

static const tusb_desc_device_t device_descriptor = {
    .bLength = sizeof(tusb_desc_device_t),
    .bDescriptorType = TUSB_DESC_DEVICE,
    .bcdUSB = 0x0200,
    .bDeviceClass = 0,
    .bDeviceSubClass = 0,
    .bDeviceProtocol = 0,
    .bMaxPacketSize0 = CFG_TUD_ENDPOINT0_SIZE,
    .idVendor = 0xCAFE,
    .idProduct = 0x2040,
    .bcdDevice = 0x0100,
    .iManufacturer = 1,
    .iProduct = 2,
    .iSerialNumber = 0,
    .bNumConfigurations = 1,
};

uint8_t const *tud_descriptor_device_cb(void) {
    return (uint8_t const *)&device_descriptor;
}

static const uint8_t report_descriptor[] = {
    TUD_HID_REPORT_DESC_KEYBOARD()
};

uint8_t const *tud_hid_descriptor_report_cb(uint8_t instance) {
    (void)instance;
    return report_descriptor;
}

enum { ITF_KEYBOARD, ITF_COUNT };
#define CONFIG_LEN (TUD_CONFIG_DESC_LEN + TUD_HID_DESC_LEN)

static const uint8_t configuration_descriptor[] = {
    TUD_CONFIG_DESCRIPTOR(1, ITF_COUNT, 0, CONFIG_LEN, 0, 100),
    TUD_HID_DESCRIPTOR(ITF_KEYBOARD, 0, HID_ITF_PROTOCOL_KEYBOARD,
                       sizeof(report_descriptor), 0x81, CFG_TUD_HID_EP_BUFSIZE, 5),
};

uint8_t const *tud_descriptor_configuration_cb(uint8_t index) {
    (void)index;
    return configuration_descriptor;
}

static const char *string_descriptors[] = {
    "", "Pico demo", "RP2040 keyboard demo",
};
static uint16_t string_buffer[32];

uint16_t const *tud_descriptor_string_cb(uint8_t index, uint16_t langid) {
    (void)langid;
    if (index == 0) {
        string_buffer[0] = (TUSB_DESC_STRING << 8) | 4;
        string_buffer[1] = 0x0409;
        return string_buffer;
    }
    if (index >= sizeof(string_descriptors) / sizeof(string_descriptors[0])) return NULL;

    const char *s = string_descriptors[index];
    uint8_t length = 0;
    while (s[length] && length < 31) {
        string_buffer[1 + length] = (uint8_t)s[length];
        ++length;
    }
    string_buffer[0] = (TUSB_DESC_STRING << 8) | (2 * length + 2);
    return string_buffer;
}
