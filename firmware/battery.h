#ifndef _BATTERY_H
#define _BATTERY_H

#ifdef __cplusplus
extern "C" {
#endif

void battery_setup();
int battery_state();
int battery_sense_read();
float battery_voltage();
int battery_percent();

#ifdef __cplusplus
}
#endif

#endif