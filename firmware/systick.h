#ifndef _SYSTICK_H
#define _SYSTICK_H

#include <stdint.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

void sys_tick_handler(void);
void wait_ms(uint32_t delay);
void systick_setup(void);

#ifdef __cplusplus
}
#endif

#endif