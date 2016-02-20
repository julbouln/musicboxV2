#ifndef _RNG_H
#define _RNG_H
#ifdef __cplusplus
  extern "C" {
#endif

void rng_setup(void);
uint32_t random_int(void);

#ifdef __cplusplus
}
#endif

#endif