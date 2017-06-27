#include <xc.h>
#include <sys/attribs.h>
#include "0x2a002a.h"

#define SD_R1 1
#define SD_R3 3
#define SD_R7 7
#define SD_RETRIES 5

u32 SD_return;
u8  SD_error = 0;

void    send_80_clk(void)
{
    u16 read16;
    u32 read32;

    SPI2CONbits.MODE32 = 1;
    SPI2BUF = 0xFFFFFFFF;
    while (SPI2STATbits.SPIBUSY) ;
    read32 = SPI2BUF;
    SPI2BUF = 0xFFFFFFFF;
    while (SPI2STATbits.SPIBUSY) ;
    read32 = SPI2BUF;
    SPI2CONbits.MODE32 = 0;
    SPI2CONbits.MODE16 = 1;
    SPI2BUF = 0xFFFF;
    while (SPI2STATbits.SPIBUSY) ;
    read16 = SPI2BUF;
    SPI2CONbits.MODE16 = 0;
}

u8  sd_command(u8 cmd, u32 arg, u8 crc, u8 restype)
{
    u8  read8;
    u32 read32;
    u8  ret;

    SS_MCP_ENCODERS = 0x0;
    SPI2BUF = 0x40 | cmd;
    while (SPI2STATbits.SPIBUSY) ;
    read8 = SPI2BUF;

    SPI2CONbits.MODE32 = 1;

    SPI2BUF = arg;
    while (SPI2STATbits.SPIBUSY) ;
    read32 = SPI2BUF;

    SPI2CONbits.MODE32 = 0;

    SPI2BUF = crc;
    while (SPI2STATbits.SPIBUSY) ;
    read8 = SPI2BUF;

    SPI2BUF = 0xFF;
    while (SPI2STATbits.SPIBUSY) ;
    ret = SPI2BUF;

    if (restype == SD_R3 || restype == SD_R7)
    {
        SPI2CONbits.MODE32 = 1;
        SPI2BUF = 0xFFFFFFFF;
        while (SPI2STATbits.SPIBUSY) ;
        SD_return = SPI2BUF;
        SPI2CONbits.MODE32 = 0;
    }
    SS_MCP_ENCODERS = 0x1;
    SPI2BUF = 0xFF;
    while (SPI2STATbits.SPIBUSY) ;
    read8 = SPI2BUF;
    return (ret);
}

void sd_card_init(void)
{
    u8  R1_response;
    u8  retries;

    SS_MCP_ENCODERS = 0x1;
    // baisser le baud rate du spi
//    SPI2BRG = 9; //divise la frequence de SCK par 20 = 2 * (9 + 1)
    SPI2BRG = 3; //divise la frequence de SCK par 6 = 2 * (2 + 1)

    // evoyer 74 coups de clk avant de baisser le cs
    send_80_clk();

    // commandes
    retries = SD_RETRIES;
    while (retries-- && (R1_response = sd_command(0, 0x0,   0x95, SD_R1)) != 0x1); // expected 0x1, repeat if necessary
    if (R1_response != 0x1)
    {
        SD_error = 1;
        return;
    }
    retries = SD_RETRIES;
    while (retries-- && (R1_response = sd_command(8, 0x1AA, 0x87, SD_R7)) != 0x1); // expected 0x1, repeat if necessary, and 0x1AA in the 32bit response
    if (R1_response != 0x1)
    {
        SD_error = 1;
        return;
    }
    // remonter le baud rate du spi
    SPI2BRG = 0; //set baudrate 1Mhz suivant 8 Mhz du pbclk

}