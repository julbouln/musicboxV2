#ifndef _MCP23S08_H
#define _MCP23S08_H
#ifdef __cplusplus
extern "C" {
#endif

#define MCP23S08_IOCON 		0x05
#define MCP23S08_IODIR		0x00
#define MCP23S08_GPPU		0x06
#define MCP23S08_GPIO		0x09
#define MCP23S08_GPINTEN	0x02
#define MCP23S08_IPOL		0x01
#define MCP23S08_DEFVAL		0x03
#define MCP23S08_INTF		0x07
#define MCP23S08_INTCAP		0x08
#define MCP23S08_OLAT		0x0A
#define MCP23S08_INTCON 	0x04


void mcp23s08_write_byte(uint8_t addr, uint8_t data);
uint8_t mcp23s08_read_byte(uint8_t reg);
uint8_t mcp23s08_read_address(uint8_t addr);
void mcp23s08_setup(uint8_t haenAdrs);

#ifdef __cplusplus
}
#endif

#endif