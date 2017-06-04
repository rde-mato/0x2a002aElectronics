#include <xc.h>
#include <sys/attribs.h>
#include "0x2a002a.h"

void MCP_write(u8 reg, u16 data)
{
    SPI2_transmit32bits(0x40000000 | (reg << 16) | data);
}

void MCP_write_8_gpio_a(u8 data)
{
    SPI2_transmit24bits(0x40, 0x12, data);
}

void MCP_write_8_gpio_b(u8 data)
{
    SPI2_transmit24bits(0x40, 0x13, data);
}
