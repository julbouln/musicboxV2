#ifndef _USBMIDI_H
#define _USBMIDI_H

#include <libopencm3/usb/usbd.h>

#ifdef __cplusplus
extern "C" {
#endif

void usbmidi_setup();
void usbmidi_write(uint8_t *msg, int len);
void usbmidi_poll();

#ifdef __cplusplus
}
#endif

#endif