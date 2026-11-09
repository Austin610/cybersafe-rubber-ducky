#include <stdbool.h>
#include <stdint.h>

#include "pico/stdlib.h"
#include "tusb.h"

uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id,
                               hid_report_type_t report_type, uint8_t *buffer,
                               uint16_t requested_length) {
    (void)instance;
    (void)report_id;
    (void)report_type;
    (void)buffer;
    (void)requested_length;
    return 0;
}

void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id,
                           hid_report_type_t report_type,
                           uint8_t const *buffer, uint16_t length) {
    (void)instance;
    (void)report_id;
    (void)report_type;
    (void)buffer;
    (void)length;
}

// Windows demo: write an original poem, then open a Spotify track.
// Key mapping assumes a US keyboard layout.
static void wait_ms_with_usb(uint32_t delay_ms) {
    absolute_time_t deadline = make_timeout_time_ms(delay_ms);
    while (!time_reached(deadline)) {
        tud_task();
        sleep_ms(1);
    }
}

static bool send_key(uint8_t modifier, uint8_t keycode) {
    uint8_t keys[6] = {keycode};
    for (uint32_t tries = 0; tries < 2000; ++tries) {
        tud_task();
        if (!tud_mounted()) return false;
        if (tud_hid_ready() && tud_hid_keyboard_report(0, modifier, keys)) {
            wait_ms_with_usb(10);
            for (uint32_t release_tries = 0; release_tries < 2000; ++release_tries) {
                tud_task();
                if (!tud_mounted()) return false;
                if (tud_hid_ready() && tud_hid_keyboard_report(0, 0, NULL)) {
                    wait_ms_with_usb(10);
                    return true;
                }
                sleep_ms(1);
            }
            return false;
        }
        sleep_ms(1);
    }
    return false;
}

static bool type_text(const char *message) {
    for (; *message; ++message) {
        uint8_t keycode;
        uint8_t modifier = 0;
        if (*message >= 'a' && *message <= 'z') {
            keycode = HID_KEY_A + (uint8_t)(*message - 'a');
        } else if (*message >= 'A' && *message <= 'Z') {
            keycode = HID_KEY_A + (uint8_t)(*message - 'A');
            modifier = KEYBOARD_MODIFIER_LEFTSHIFT;
        } else if (*message >= '1' && *message <= '9') {
            keycode = HID_KEY_1 + (uint8_t)(*message - '1');
        } else if (*message == '0') {
            keycode = HID_KEY_0;
        } else if (*message == ' ') {
            keycode = HID_KEY_SPACE;
        } else if (*message == ':') {
            keycode = HID_KEY_SEMICOLON;
            modifier = KEYBOARD_MODIFIER_LEFTSHIFT;
        } else if (*message == '\n') {
            keycode = HID_KEY_ENTER;
        } else {
            return false;
        }
        if (!send_key(modifier, keycode)) return false;
    }
    return true;
}

int main(void) {
    tusb_init();
    while (!tud_mounted()) {
        tud_task();
        sleep_ms(1);
    }
    wait_ms_with_usb(1500);

    bool run_dialog_opened = send_key(KEYBOARD_MODIFIER_LEFTGUI, HID_KEY_R);
    wait_ms_with_usb(1000);
    if (run_dialog_opened && type_text("notepad") &&
        send_key(0, HID_KEY_ENTER)) {
        wait_ms_with_usb(6000);
        if (send_key(KEYBOARD_MODIFIER_LEFTCTRL, HID_KEY_N)) {
            wait_ms_with_usb(1000);
            if (type_text("on the edge\n\n"
                          "at the edge of the sleeping city\n"
                          "the streetlights shiver in the rain\n"
                          "i hear your name beneath the traffic\n"
                          "a distant echo in my veins\n\n"
                          "the night unfolds a black horizon\n"
                          "and every road runs out of ground\n"
                          "i reach for you across the silence\n"
                          "but only hear the river sound\n\n"
                          "below the bridge the water mirrors\n"
                          "a thousand windows burning blue\n"
                          "each one a life that keeps on moving\n"
                          "while mine is standing still with you\n\n"
                          "if we are falling through the darkness\n"
                          "let one small light refuse to fade\n"
                          "i leave it shining on the water\n"
                          "for all the words we never said")) {
                wait_ms_with_usb(1000);
                if (send_key(KEYBOARD_MODIFIER_LEFTGUI, HID_KEY_R)) {
                    wait_ms_with_usb(1000);
                    if (type_text("spotify:track:5ZDKPFRZC6QlJpf8bCIXTs")) {
                        if (send_key(0, HID_KEY_ENTER)) {
                            // A link may only navigate when another track is playing.
                            // Pause the current playback, then open the track again.
                            wait_ms_with_usb(5000);
                            if (send_key(0, HID_KEY_SPACE)) {
                                wait_ms_with_usb(500);
                                if (send_key(KEYBOARD_MODIFIER_LEFTGUI, HID_KEY_R)) {
                                    wait_ms_with_usb(1000);
                                    if (type_text("spotify:track:5ZDKPFRZC6QlJpf8bCIXTs")) {
                                        send_key(0, HID_KEY_ENTER);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    while (true) {
        tud_task();
        sleep_ms(1);
    }
}
