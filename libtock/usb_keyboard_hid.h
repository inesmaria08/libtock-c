#pragma once

#include "tock.h"

#ifdef __cplusplus
extern "C" {
#endif

#define DRIVER_NUM_USBKEYBOARDHID 0x90005

int usb_keyboard_hid_set_callback(subscribe_upcall callback, void* callback_args);
int usb_keyboard_hid_set_receive_buffer(uint8_t* buffer, uint32_t len);
int usb_keyboard_hid_set_send_buffer(uint8_t* buffer, uint32_t len);
int usb_keyboard_hid_send(void);

int usb_keyboard_hid_send_sync(uint8_t* buffer, uint32_t len);

// Send one ASCII character. `buffer` must be at least 64 bytes.
int usb_keyboard_hid_send_letter_sync(uint8_t* buffer, char letter);

// Send an array of ASCII characters. `buffer` must be at least 64 bytes.
int usb_keyboard_hid_send_string_sync(uint8_t* buffer, char* str, int length);


#ifdef __cplusplus
}
#endif
