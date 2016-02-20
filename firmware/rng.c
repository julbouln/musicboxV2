#include <stdint.h>

#include <libopencm3/cm3/common.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/f4/rng.h>

#include "rng.h"

#ifdef __cplusplus
  extern "C" {
#endif

void rng_setup(void)
{
	/* Enable interupt */
	/* Set the IE bit in the RNG_CR register. */
	RNG_CR |= RNG_CR_IE;
	/* Enable the random number generation by setting the RNGEN bit in
	   the RNG_CR register. This activates the analog part, the RNG_LFSR
	   and the error detector.
	*/
	RNG_CR |= RNG_CR_RNGEN;
}


/* Tried to folow the guidelines in the stm32f4 user manual.*/
uint32_t random_int(void)
{
	static uint32_t last_value;
	static uint32_t new_value;

	uint32_t error_bits = 0;
	error_bits = RNG_SR_SEIS | RNG_SR_CEIS;
	while (new_value == last_value) {
		/* Check for error flags and if data is ready. */
		if (((RNG_SR & error_bits) == 0) &&
		    ((RNG_SR & RNG_SR_DRDY) == 1)) {
			new_value = RNG_DR;
		}
	}
	last_value = new_value;
	return new_value;
}

#ifdef __cplusplus
}
#endif
