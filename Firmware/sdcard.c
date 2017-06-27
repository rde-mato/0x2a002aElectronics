#include <xc.h>
#include <sys/attribs.h>
#include "0x2a002a.h"

u8 sd_read;
u32 sd_read32;

void sd_card_init(void)
{
    SS_MCP_ENCODERS = 0x0;
    SPI2BUF = 0x40;
    while (SPI2STATbits.SPIBUSY) ;
    sd_read = SPI2BUF;
    SPI2CONbits.MODE32 = 1;
    SPI2BUF = 0x00000000;
    while (SPI2STATbits.SPIBUSY) ;
    sd_read32 = SPI2BUF;
    SPI2CONbits.MODE32 = 0;
    SPI2BUF = 0x95;
    while (SPI2STATbits.SPIBUSY) ;
    sd_read = SPI2BUF;
    SPI2BUF = 0xFF;
    while (SPI2STATbits.SPIBUSY) ;
    sd_read = SPI2BUF;
    SS_MCP_ENCODERS = 0x1;
    SPI2BUF = 0xFF;
    while (SPI2STATbits.SPIBUSY) ;
    sd_read = SPI2BUF;


    SS_MCP_ENCODERS = 0x0;
    SPI2BUF = 0x48;
    while (SPI2STATbits.SPIBUSY) ;
    sd_read = SPI2BUF;
    SPI2CONbits.MODE32 = 1;
    SPI2BUF = 0x000001AA;
    while (SPI2STATbits.SPIBUSY) ;
    sd_read32 = SPI2BUF;
    SPI2CONbits.MODE32 = 0;
    SPI2BUF = 0x04;
    while (SPI2STATbits.SPIBUSY) ;
    sd_read = SPI2BUF;
    SPI2BUF = 0xFF;
    while (SPI2STATbits.SPIBUSY) ;
    sd_read = SPI2BUF;
    SPI2CONbits.MODE32 = 1;
    SPI2BUF = 0xFFFFFFFF;
    while (SPI2STATbits.SPIBUSY) ;
    sd_read32 = SPI2BUF;
    SPI2CONbits.MODE32 = 0;
    SS_MCP_ENCODERS = 0x1;
}