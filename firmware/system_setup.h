#ifndef _SYSTEM_SETUP_H
#define _SYSTEM_SETUP_H

#ifdef __cplusplus
extern "C" {
#endif

void jack_sense(void);
void speaker_shutdown();
void speaker_start();

void system_setup(void);

#ifdef __cplusplus
}
#endif

#endif