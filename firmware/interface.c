
#include <libopencm3/stm32/spi.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/exti.h>

#include "drivers_config.h"
#include "mcp23s08.h"
#include "ssd1306.h"
#include "interface.h"

#ifdef __cplusplus
extern "C" {
#endif

uint8_t _pad_gpio = 0;
uint8_t _pad_pressed = 0;

uint16_t exti_line_state;
/*
void exti0_isr(void)
{
	_pad_pressed++;

	exti_reset_request(EXTI0);
}
*/

// setup screen and pad
void interface_setup() {
	// SPI
	gpio_mode_setup(INTERFACE_SPI_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, INTERFACE_SPI_CLK_PIN | INTERFACE_SPI_MISO_PIN | INTERFACE_SPI_MOSI_PIN );
	gpio_set_af(INTERFACE_SPI_PORT, GPIO_AF5, INTERFACE_SPI_CLK_PIN | INTERFACE_SPI_MISO_PIN | INTERFACE_SPI_MOSI_PIN);

	gpio_mode_setup(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO0 | GPIO1);

	gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO4 | GPIO5);

	rcc_periph_clock_enable(RCC_SPI1);
	spi_init_master(SPI1, SPI_CR1_BAUDRATE_FPCLK_DIV_4,
	                SPI_CR1_CPOL_CLK_TO_0_WHEN_IDLE,
	                SPI_CR1_CPHA_CLK_TRANSITION_1,
	                SPI_CR1_DFF_8BIT,
	                SPI_CR1_MSBFIRST);
	spi_enable_ss_output(SPI1);
	spi_enable(SPI1);	

	// OLED
	ssd1306_setup();
	// PAD
	pad_setup();

	nvic_set_priority(NVIC_EXTI0_IRQ,0x00);
	/* Enable EXTI0 interrupt. */
	nvic_enable_irq(NVIC_EXTI0_IRQ);

	gpio_mode_setup(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO0);

	/* Configure the EXTI subsystem. */
	exti_select_source(EXTI0, GPIOA);
	exti_set_trigger(EXTI0, EXTI_TRIGGER_FALLING);
	exti_enable_request(EXTI0);
}


void pad_setup() {
	mcp23s08_setup(0x20);
	mcp23s08_write_byte(MCP23S08_IOCON, 0b00100000); //chip configure,serial, HAEN, etc.
	mcp23s08_write_byte(MCP23S08_IODIR, 0b00011111); //pin0..4(in) / 5...7(out)
	mcp23s08_write_byte(MCP23S08_GPPU, 0b00011111); //pull up resistors
	mcp23s08_write_byte(MCP23S08_DEFVAL, 0b00011111); //default comparison value for pin 0..4
	mcp23s08_write_byte(MCP23S08_GPINTEN, 0b00011111); //int on change enable on pin 0..4
	mcp23s08_write_byte(MCP23S08_INTCON, 0b00011111); //int on change control on pin 0..4

	mcp23s08_read_byte(MCP23S08_INTCAP);//clear int register
}


uint8_t pad_read() {
	_pad_gpio = mcp23s08_read_byte(MCP23S08_GPIO);
	return _pad_gpio;
}

uint8_t pad_pressed() {
	return _pad_pressed;
}

void pad_clear() {
	_pad_pressed=0;
}

#ifdef __cplusplus
}
#endif
