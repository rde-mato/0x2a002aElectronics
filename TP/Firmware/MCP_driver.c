#include <xc.h>
#include <sys/attribs.h>
#include "0x2a002a.h"

extern u16  SPI2_write_buf[10];
extern u8  SPI2_write_buf_size;

void    MCP_init()
{
    SPI2_write_buf[0] = 0x4102;
    SPI2_write_buf[1] = 0x4100;
    SPI2_write_buf[2] = 0x4113;
    SPI2_write_buf_size = 3;
    SPI2_send();
    return ;
}