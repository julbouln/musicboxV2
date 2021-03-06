#ifndef _DRIVERS_CONFIG_H
#define _DRIVERS_CONFIG_H

#define SAM2695_MIDI_IN_PORT GPIOC
#define SAM2695_MIDI_IN_PIN GPIO10
#define SAM2695_RESET_PORT GPIOC
#define SAM2695_RESET_PIN GPIO11

#define INTERFACE_SPI SPI1
#define INTERFACE_SPI_PORT GPIOA
#define INTERFACE_SPI_CLK_PIN GPIO5
#define INTERFACE_SPI_MISO_PIN GPIO6
#define INTERFACE_SPI_MOSI_PIN GPIO7

#define SSD1306_SPI INTERFACE_SPI
#define SSD1306_CS_PORT GPIOB
#define SSD1306_CS_PIN GPIO0
#define SSD1306_RST_PORT GPIOC
#define SSD1306_RST_PIN GPIO4
#define SSD1306_DC_PORT GPIOC
#define SSD1306_DC_PIN GPIO5

#define MCP23S08_SPI INTERFACE_SPI
#define MCP23S08_CS_PORT GPIOB
#define MCP23S08_CS_PIN GPIO1

#define BAT_SENSE_ADC ADC1

#define BAT_SENSE_PORT GPIOC
#define BAT_SENSE_PIN GPIO0
#define BAT_STAT_PORT GPIOB
#define BAT_STAT1_PORT BAT_STAT_PORT
#define BAT_STAT1_PIN GPIO3
#define BAT_STAT2_PORT BAT_STAT_PORT
#define BAT_STAT2_PIN GPIO4
#define BAT_PG_PORT BAT_STAT_PORT
#define BAT_PG_PORT GPIO5

#define JACK_SENSE_PORT GPIOB
#define JACK_SENSE_PIN GPIO12
#define AMP_SHDN_PORT GPIOB
#define AMP_SHDN_PIN GPIO13

#endif
