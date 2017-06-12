#include <xc.h>
#include <sys/attribs.h>
#include "0x2a002a.h"

//void MCP_write(u8 reg, u16 data)
//{
//    SPI2_transmit32bits(0x40000000 | (reg << 16) | data);
//}

void MCP_write_8_gpio_a(u8 data)
{
    u8  buf[3] = {0x40, 0x12, data};

    SPI2_push_buffer(E_SPI2_SS_MCP_LCD, buf, 3);
}

void MCP_write_8_gpio_b(u8 data)
{
    u8  buf[3] = {0x40, 0x13, data};

    SPI2_push_buffer(E_SPI2_SS_MCP_LCD, buf, 3);
}
