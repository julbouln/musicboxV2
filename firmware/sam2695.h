#ifndef _SAM2695_H
#define _SAM2695_H

#ifdef __cplusplus
extern "C" {
#endif

void sam2695_setup(void);
void sam2695_reset(void);
void sam2695_shutdown(void);
void sam2695_write(char c);

#ifdef __cplusplus
}
#endif

#endif