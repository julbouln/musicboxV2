
#include <stdio.h>
#include <stdint.h>

#include <libopencm3/stm32/spi.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/cm3/nvic.h>

#include "drivers_config.h"
#include "systick.h"
#include "mcp23s08.h"

#define INPUT 0
#define OUTPUT 1

#define LOW 0
#define HIGH 1

#ifdef __cplusplus
extern "C" {
#endif

uint8_t 		_adrs;
uint8_t 		_useHaen;
uint8_t 		_readCmd;
uint8_t 		_writeCmd;
uint8_t			_gpioDirection;
uint8_t			_gpioState;

#define CS_CLEAR gpio_clear(MCP23S08_CS_PORT, MCP23S08_CS_PIN);
#define CS_SET gpio_set(MCP23S08_CS_PORT, MCP23S08_CS_PIN);

void mcp23s08_start_send(uint8_t mode){
	CS_CLEAR
	mode == 1 ? spi_xfer(MCP23S08_SPI,_readCmd) : spi_xfer(MCP23S08_SPI,_writeCmd);
}

void mcp23s08_end_send(){
	CS_SET
}

void mcp23s08_write_byte(uint8_t addr, uint8_t data) {
	mcp23s08_start_send(0);
	spi_xfer(MCP23S08_SPI, addr);
	spi_xfer(MCP23S08_SPI, data);
	mcp23s08_end_send();
}


uint8_t mcp23s08_read_byte(uint8_t reg){
  uint8_t data = 0;
  mcp23s08_start_send(1);
  spi_xfer(MCP23S08_SPI,reg);
  data = (uint8_t)spi_xfer(MCP23S08_SPI,0x00);
  mcp23s08_end_send();
  return data;
}

uint8_t mcp23s08_read_address(uint8_t addr){
	uint8_t low_byte = 0x00;
	mcp23s08_start_send(1);
	spi_xfer(MCP23S08_SPI,addr);
	low_byte = (uint8_t)spi_xfer(MCP23S08_SPI,0x00);
	mcp23s08_end_send();
	return low_byte;
}


void mcp23s08_setup(uint8_t haenAdrs) {
	if (haenAdrs >= 0x20 && haenAdrs <= 0x23) { //HAEN works between 0x20...0x23
		_adrs = haenAdrs;
		_useHaen = 1;
	} else {
		_adrs = 0;
		_useHaen = 0;
	}
	_readCmd =  (_adrs << 1) | 1;
	_writeCmd = _adrs << 1;
	//setup register values for this chip
	
	CS_SET
	wait_ms(100);
	_useHaen == 1 ? mcp23s08_write_byte(MCP23S08_IOCON, 0b00101000) : mcp23s08_write_byte(MCP23S08_IOCON, 0b00100000);
	
	_gpioDirection = 0xFF;//all in
	_gpioState = 0x00;//all low
}

#ifdef __cplusplus
}
#endif
