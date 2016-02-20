#ifndef _INTERFACE_H
#define _INTERFACE_H

#ifdef __cplusplus
extern "C" {
#endif

void interface_setup();


/* PAD */
#define PAD_TOP 0b10111
#define PAD_BOTTOM 0b11110
#define PAD_PREVIOUS 0b11101
#define PAD_NEXT 0b01111
#define PAD_SELECT 0b11011

void pad_setup();
uint8_t pad_read();
uint8_t pad_pressed();
void pad_clear();

#ifdef __cplusplus
}
#endif


#endif