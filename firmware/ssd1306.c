#include "drivers_config.h"
#include "systick.h"
#include "ssd1306.h"

#include <libopencm3/stm32/spi.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/cm3/nvic.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SSD1306_SETCONTRAST 0x81
#define SSD1306_DISPLAYALLON_RESUME 0xA4
#define SSD1306_DISPLAYALLON 0xA5
#define SSD1306_NORMALDISPLAY 0xA6
#define SSD1306_INVERTDISPLAY 0xA7
#define SSD1306_DISPLAYOFF 0xAE
#define SSD1306_DISPLAYON 0xAF
#define SSD1306_SETDISPLAYOFFSET 0xD3
#define SSD1306_SETCOMPINS 0xDA
#define SSD1306_SETVCOMDETECT 0xDB
#define SSD1306_SETDISPLAYCLOCKDIV 0xD5
#define SSD1306_SETPRECHARGE 0xD9
#define SSD1306_SETMULTIPLEX 0xA8
#define SSD1306_SETLOWCOLUMN 0x00
#define SSD1306_SETHIGHCOLUMN 0x10
#define SSD1306_SETSTARTLINE 0x40
#define SSD1306_MEMORYMODE 0x20
#define SSD1306_COMSCANINC 0xC0
#define SSD1306_COMSCANDEC 0xC8
#define SSD1306_SEGREMAP 0xA0
#define SSD1306_CHARGEPUMP 0x8D


#define CS_CLEAR gpio_clear(SSD1306_CS_PORT, SSD1306_CS_PIN);
#define CS_SET gpio_set(SSD1306_CS_PORT, SSD1306_CS_PIN);

#define DC_CLEAR gpio_clear(SSD1306_DC_PORT, SSD1306_DC_PIN);
#define DC_SET gpio_set(SSD1306_DC_PORT, SSD1306_DC_PIN);

#define RST_CLEAR gpio_clear(SSD1306_RST_PORT, SSD1306_RST_PIN);
#define RST_SET gpio_set(SSD1306_RST_PORT, SSD1306_RST_PIN);

#define SSD1306_EXTERNALVCC 0x1
#define SSD1306_SWITCHCAPVCC 0x2

#ifndef _BV
#define _BV(bit) (1<<(bit))
#endif

uint8_t buffer[(SSD1306_WIDTH * SSD1306_HEIGHT) / 8] = {0};

static void ssd1306_command(uint8_t c)
{
	CS_SET
	DC_CLEAR
	CS_CLEAR
	spi_xfer(SSD1306_SPI, c);
	CS_SET
};

void ssd1306_setup() {
	uint8_t vccstate = SSD1306_SWITCHCAPVCC;
	
	RST_SET
	// VDD (3.3V) goes high at start, lets just chill for a ms
	wait_ms(1);
	// bring reset low
	RST_CLEAR
	// wait 10ms
	wait_ms(10);
	// bring out of reset
	RST_SET
	// turn on VCC (9V?)

	ssd1306_command(SSD1306_DISPLAYOFF);
	ssd1306_command(SSD1306_SETDISPLAYCLOCKDIV);
	ssd1306_command(0x80);                                  // the suggested ratio 0x80

	ssd1306_command(SSD1306_SETMULTIPLEX);
	ssd1306_command(SSD1306_HEIGHT - 1);

	ssd1306_command(SSD1306_SETDISPLAYOFFSET);
	ssd1306_command(0x0);                                   // no offset

	ssd1306_command(SSD1306_SETSTARTLINE | 0x0);            // line #0

	ssd1306_command(SSD1306_CHARGEPUMP);
	ssd1306_command((vccstate == SSD1306_EXTERNALVCC) ? 0x10 : 0x14);

	ssd1306_command(SSD1306_MEMORYMODE);
	ssd1306_command(0x00);                                  // 0x0 act like ks0108

	ssd1306_command(SSD1306_SEGREMAP | 0x1);

	ssd1306_command(SSD1306_COMSCANDEC);

	ssd1306_command(SSD1306_SETCOMPINS);
	ssd1306_command(SSD1306_HEIGHT == 32 ? 0x02 : 0x12);        // TODO - calculate based on _rawHieght ?

	ssd1306_command(SSD1306_SETCONTRAST);
	ssd1306_command(SSD1306_HEIGHT == 32 ? 0x8F : ((vccstate == SSD1306_EXTERNALVCC) ? 0x9F : 0xCF) );

	ssd1306_command(SSD1306_SETPRECHARGE);
	ssd1306_command((vccstate == SSD1306_EXTERNALVCC) ? 0x22 : 0xF1);

	ssd1306_command(SSD1306_SETVCOMDETECT);
	ssd1306_command(0x40);

	ssd1306_command(SSD1306_DISPLAYALLON_RESUME);

	ssd1306_command(SSD1306_NORMALDISPLAY);

	ssd1306_command(SSD1306_DISPLAYON);

}

void ssd1306_drawpixel(int x, int y, uint16_t color) {
//    if ((x < 0) || (x >= SSD1306_WIDTH) || (y < 0) || (y >= SSD1306_HEIGHT))
//        return;

	// x is which column
	if (color == OLED_WHITE)
		buffer[x + (y / 8)*SSD1306_WIDTH] |= _BV((y % 8));
	else // else black
		buffer[x + (y / 8)*SSD1306_WIDTH] &= ~_BV((y % 8));
}

void sendDisplayBuffer()
{
	uint16_t i;
	CS_SET
	DC_SET
	CS_CLEAR

	for (i = 0; i < (SSD1306_WIDTH * SSD1306_HEIGHT) / 8; i++)
		spi_xfer(SSD1306_SPI, buffer[i]);

	if (SSD1306_HEIGHT == 32)
	{
		for (i = 0; i < (SSD1306_WIDTH * SSD1306_HEIGHT) / 8; i++)
			spi_xfer(SSD1306_SPI, 0);
	}

	CS_SET
}

void ssd1306_display(void)
{
	ssd1306_command(SSD1306_SETLOWCOLUMN | 0x0);  // low col = 0
	ssd1306_command(SSD1306_SETHIGHCOLUMN | 0x0);  // hi col = 0
	ssd1306_command(SSD1306_SETSTARTLINE | 0x0); // line #0
	sendDisplayBuffer();
}

#ifdef __cplusplus
}
#endif
