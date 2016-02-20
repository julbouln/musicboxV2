#include "system_setup.h"
#include "systick.h"
#include "rng.h"
#include "interface.h"
#include "usbmidi.h"
#include "ssd1306.h"
#include "gfx.h"
#include "battery.h"
#include "drivers_config.h"

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/usb/usbd.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/exti.h>

#ifdef __cplusplus
extern "C" {
#endif

static void usb_setup(void) {
    /* USB pins */
    gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE,
                    GPIO9 | GPIO11 | GPIO12);
    gpio_set_af(GPIOA, GPIO_AF10, GPIO9 | GPIO11 | GPIO12);

}

static void clock_setup(void) {
    rcc_clock_setup_hse_3v3 ( &rcc_hse_12mhz_3v3[RCC_CLOCK_3V3_168MHZ]);

    rcc_periph_clock_enable(RCC_GPIOA);
    rcc_periph_clock_enable(RCC_GPIOB);
    rcc_periph_clock_enable(RCC_GPIOC);
    rcc_periph_clock_enable(RCC_USART3); // sam2695
    rcc_periph_clock_enable(RCC_OTGFS); // usb

    rcc_periph_clock_enable(RCC_SYSCFG); // pad interrupt ?

    rcc_periph_clock_enable(RCC_RNG); // rng

    rcc_periph_clock_enable(RCC_ADC1); // battery sense
}

// 12mhz MCO for SAM2695 clock
static void mco_setup(void) {
    gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO8);
    gpio_set_output_options(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_100MHZ, GPIO8);
    gpio_set_af(GPIOA, GPIO_AF0, GPIO8);
    rcc_set_mco(RCC_CFGR_MCO1_HSE);
}

void speaker_setup() {
    gpio_mode_setup(AMP_SHDN_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, AMP_SHDN_PIN);
}

void speaker_shutdown() { 
    gpio_clear(AMP_SHDN_PORT, AMP_SHDN_PIN);
}

void speaker_start() {
    gpio_set(AMP_SHDN_PORT,AMP_SHDN_PIN);
}

void exti1_isr(void)
{

    if (gpio_get(JACK_SENSE_PORT,JACK_SENSE_PIN) != 0) {
        speaker_shutdown();
    } else {
        speaker_start();
    }

    exti_reset_request(EXTI1);
}

void jack_setup() {

    nvic_set_priority(NVIC_EXTI1_IRQ,0x00);
    /* Enable EXTI1 interrupt. */
    nvic_enable_irq(NVIC_EXTI1_IRQ);

    gpio_mode_setup(JACK_SENSE_PORT, GPIO_MODE_INPUT, GPIO_PUPD_NONE, JACK_SENSE_PIN);

    /* Configure the EXTI subsystem. */
    exti_select_source(EXTI1, JACK_SENSE_PORT);
    exti_set_trigger(EXTI1, EXTI_TRIGGER_BOTH);
    exti_enable_request(EXTI1);
}

void jack_sense() {
    if (gpio_get(JACK_SENSE_PORT,JACK_SENSE_PIN) != 0) {
        speaker_shutdown();
    } else {
        speaker_start();
    }
}

void system_setup(void)
{
    clock_setup();
    mco_setup();
    usb_setup();
    systick_setup();
    rng_setup();
    battery_setup();

    speaker_setup();
    jack_setup();
    // setup pad and screen
    interface_setup();

    // setup and reset SAM2695
    sam2695_setup();
    wait_ms(100);
    sam2695_reset();

    usbmidi_setup();

    // init gfx and fill screen
    gfx_init(ssd1306_drawpixel, SSD1306_WIDTH, SSD1306_HEIGHT, GFX_FONT_SMALL);
    gfx_setRotation(GFX_ROT_180);
    gfx_fillScreen(OLED_BLACK);
    gfx_setTextColor(OLED_WHITE, OLED_BLACK);
//    gfx_setTextWrap(1);
    gfx_setTextSize(1);
    ssd1306_display();
}

#ifdef __cplusplus
}
#endif
