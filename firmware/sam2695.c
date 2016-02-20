#include "drivers_config.h"
#include "sam2695.h"
#include "systick.h"

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>

#ifdef __cplusplus
extern "C" {
#endif


void sam2695_setup(void) {
    /* Sam2695 */
    /* midi */
    gpio_mode_setup(SAM2695_MIDI_IN_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, SAM2695_MIDI_IN_PIN);
    gpio_set_af(SAM2695_MIDI_IN_PORT, GPIO_AF7, SAM2695_MIDI_IN_PIN);

    /* Setup Midi USART parameters. */
    usart_set_baudrate(USART3, 31250);
    usart_set_databits(USART3, 8);
    usart_set_stopbits(USART3, USART_STOPBITS_1);
    usart_set_mode(USART3, USART_MODE_TX);
    usart_set_parity(USART3, USART_PARITY_NONE);
    usart_set_flow_control(USART3, USART_FLOWCONTROL_NONE);

    /* Finally enable the USART. */
    usart_enable(USART3);

    /* reset */
    gpio_mode_setup(SAM2695_RESET_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, SAM2695_RESET_PIN);

}

void sam2695_reset(void) {
    gpio_clear(SAM2695_RESET_PORT, SAM2695_RESET_PIN);
    wait_ms(10);
    gpio_set(SAM2695_RESET_PORT, SAM2695_RESET_PIN);
    wait_ms(100);
}


void sam2695_shutdown(void) {
    gpio_clear(SAM2695_RESET_PORT, SAM2695_RESET_PIN);
}

void sam2695_write(char c) {
    usart_send_blocking(USART3, c);
}

#ifdef __cplusplus
}
#endif
