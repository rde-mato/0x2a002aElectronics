#include <xc.h>
#include <sys/attribs.h>
#include "0x2a002a.h"

void MCP_write(u8 reg, u16 data)
{
    SPI2_transmit32bits(0x40000000 | (reg << 16) | (data & 0xFFFF));
}

u16 MCP_read_gpio(void)
{
    return (SPI2_transmit32bits(0x4112FFFF) & 0xFFFF);
}