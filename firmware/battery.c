#include "drivers_config.h"

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/adc.h>

#ifdef __cplusplus
extern "C" {
#endif

void battery_setup() {
	gpio_mode_setup(BAT_STAT_PORT, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, BAT_STAT1_PORT | BAT_STAT2_PORT | BAT_PG_PORT);

	gpio_mode_setup(BAT_SENSE_PORT, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, BAT_SENSE_PIN);

	adc_off(BAT_SENSE_ADC);
	adc_disable_scan_mode(BAT_SENSE_ADC);
	adc_set_single_conversion_mode(ADC1);
	adc_set_sample_time(ADC1, ADC_CHANNEL10, ADC_SMPR_SMP_15CYC);
	uint8_t channels[] = {ADC_CHANNEL10};
	adc_set_regular_sequence(BAT_SENSE_ADC, 1, channels);

	adc_power_on(BAT_SENSE_ADC);
}

int battery_state() {
	return gpio_get(BAT_STAT_PORT, BAT_STAT1_PORT | BAT_STAT2_PORT | BAT_PG_PORT);
}

uint16_t battery_sense_read() {
	adc_start_conversion_regular(BAT_SENSE_ADC);
	while (!adc_eoc(BAT_SENSE_ADC));
	return adc_read_regular(BAT_SENSE_ADC);
}

/*
float battery_voltage() {
	return ((battery_sense_read() * 3.3) / 0.32);
}

int battery_percent() {
	int sense_samples = 512;
	float v = 0;
	int i;
	for (i = 0; i < sense_samples; i++) {
		v += battery_voltage();
	}

	int p = (v / sense_samples * 1000.0 - 3100.0) / (4200.0 - 3100.0) * 100;

	if (p < 0)
		p = 0;
	if (p > 100)
		p = 100;
	return p;
}
*/

float battery_voltage() {
	return (battery_sense_read() * 0.0024);
}


int battery_percent() {
	float v = 0;
	v = battery_voltage();

	int p = (v / 4.2) * 100;

	if (p < 0)
		p = 0;
	if (p > 100)
		p = 100;
	return p;
}
#ifdef __cplusplus
}
#endif
