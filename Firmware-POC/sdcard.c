#include <xc.h>
#include <sys/attribs.h>
#include "0x2a002a.h"

u32 SD_return;
u8  SD_error = 0;
u8  SD_type = SD_SDSC;

u8  SD_read_buf[SD_BLOCK_SIZE] = {0};
u8  SD_write_buf[SD_BLOCK_SIZE] = "youpi youpla";

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

//u8  sd_command(u8 cmd, u32 arg, u8 crc, u8 restype)
//{
//    u8  read8;
//    u32 read32;
//    u8  ret;
//
//    SS_MCP_ENCODERS = 0x0;
//    SPI2BUF = 0x40 | cmd;
//    while (SPI2STATbits.SPIBUSY) ;
//    read8 = SPI2BUF;
//
//    SPI2CONbits.MODE32 = 1;
//
//    SPI2BUF = arg;
//    while (SPI2STATbits.SPIBUSY) ;
//    read32 = SPI2BUF;
//
//    SPI2CONbits.MODE32 = 0;
//
//    SPI2BUF = crc;
//    while (SPI2STATbits.SPIBUSY) ;
//    read8 = SPI2BUF;
//
//    SPI2BUF = 0xFF;
//    while (SPI2STATbits.SPIBUSY) ;
//    ret = SPI2BUF;
//
//    if (restype == SD_R3 || restype == SD_R7)
//    {
//        SPI2CONbits.MODE32 = 1;
//        SPI2BUF = 0xFFFFFFFF;
//        while (SPI2STATbits.SPIBUSY) ;
//        SD_return = SPI2BUF;
//        SPI2CONbits.MODE32 = 0;
//    }
//    SS_MCP_ENCODERS = 0x1;
//    SPI2BUF = 0xFF;
//    while (SPI2STATbits.SPIBUSY) ;
//    read8 = SPI2BUF;
//    return (ret);
//}

u8  SD_command_retry(u8 cmd, u32 arg, u8 crc, u8 restype)
{
    u8  read8;
    u32 read32;
    u8  ret = 0xFF;
    u8  retries = SD_RETRIES;

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

    while (retries--)
    {
        SPI2BUF = 0xFF;
        while (SPI2STATbits.SPIBUSY) ;
        if ((ret = SPI2BUF) != 0xFF)
            break;
    }
    if (ret != 0xFF && (restype == SD_R3 || restype == SD_R7))
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

void SD_card_init(void)
{
    u8  R1_response;
    u8  retries = SD_RETRIES;

    // baisser le baud rate du spi
//    SPI2BRG = 9; //divise la frequence de SCK par 20 = 2 * (9 + 1)
    SPI2BRG = 3; //divise la frequence de SCK par 6 = 2 * (2 + 1)
    send_80_clk();
    R1_response = SD_command_retry(0, 0x0,   0x95, SD_R1);
    R1_response = SD_command_retry(8, 0x1AA, 0x87, SD_R7);
    R1_response = SD_command_retry(58, 0x0,  0x01, SD_R3);  // reads the OCR register to know the voltage and therefore if the card is SDSC, SDHC or SDXC

    while (retries--)
    {
        R1_response = SD_command_retry(55, 0x0,  0x65, SD_R1);
        R1_response = SD_command_retry(41, 0x0,  0xE5, SD_R1);
        if (R1_response == 0x00)
            break ;
    }
    R1_response = SD_command_retry(58, 0x0, 0x01, SD_R3);  // reads the OCR register, if bit 30 of OCR is 0 then SDSC else SDHC or SDXC
    if (SD_return & (1 << 30))
        SD_type = SD_SDHC;
    R1_response = SD_command_retry(16, SD_BLOCK_SIZE, 0x01, SD_R1);  // set block length to 512
    // remonter le baud rate du spi
    SPI2BRG = 0; //set baudrate 1Mhz suivant 8 Mhz du pbclk
}

u8  SD_card_read_block(u32 block)
{
    u8  R1;
    u8  token;
    u8  read8;
    u32 read32;
    u32  retries = SD_RETRIES;
    u16 i = 0;

    SS_MCP_ENCODERS = 0x0;
    SPI2BUF = 0x40 | 17;
    while (SPI2STATbits.SPIBUSY) ;
    read8 = SPI2BUF;

    SPI2CONbits.MODE32 = 1;

    if (SD_type == SD_SDSC)
        block *= SD_BLOCK_SIZE;
    SPI2BUF = block;
    while (SPI2STATbits.SPIBUSY) ;
    read32 = SPI2BUF;

    SPI2CONbits.MODE32 = 0;

    SPI2BUF = 0x01;
    while (SPI2STATbits.SPIBUSY) ;
    read8 = SPI2BUF;

    while (retries--)
    {
        SPI2BUF = 0xFF;
        while (SPI2STATbits.SPIBUSY) ;
        if ((R1 = SPI2BUF) != 0xFF)
            break;
    }
    if (R1 != 0x00)
        return (SD_READ_ERROR_WRONG_R1);

    retries = SD_RETRIES;
    while (retries--)
    {
        SPI2BUF = 0xFF;
        while (SPI2STATbits.SPIBUSY) ;
        if ((token = SPI2BUF) == 0b11111110)
            break;
    }
    if (token != 0b11111110)
        return (SD_READ_ERROR_WRONG_TOKEN);
    while (i < SD_BLOCK_SIZE)
    {
        SPI2BUF = 0xFF;
        while (SPI2STATbits.SPIBUSY) ;
        SD_read_buf[i++] = SPI2BUF;
    }
    SPI2BUF = 0xFF;
    while (SPI2STATbits.SPIBUSY) ;
    read8 = SPI2BUF;
    SPI2BUF = 0xFF;
    while (SPI2STATbits.SPIBUSY) ;
    read8 = SPI2BUF;

    SS_MCP_ENCODERS = 0x1;
    SPI2BUF = 0xFF;
    while (SPI2STATbits.SPIBUSY) ;
    read8 = SPI2BUF;
    return (SD_READ_NO_ERROR);
}

u8  SD_card_write_block(u32 block)
{
    u8  R1;
    u8  read8;
    u32 read32;
    u8  retries = SD_RETRIES;
    u8 data_response;
    u8 response_status;
    u16 i = 0;

    SS_MCP_ENCODERS = 0x0;
    SPI2BUF = 0x40 | 24;
    while (SPI2STATbits.SPIBUSY) ;
    read8 = SPI2BUF;

    SPI2CONbits.MODE32 = 1;

    if (SD_type == SD_SDSC)
        block *= SD_BLOCK_SIZE;
    SPI2BUF = block;
    while (SPI2STATbits.SPIBUSY) ;
    read32 = SPI2BUF;

    SPI2CONbits.MODE32 = 0;

    SPI2BUF = 0x01;
    while (SPI2STATbits.SPIBUSY) ;
    read8 = SPI2BUF;

    while (retries--)
    {
        SPI2BUF = 0xFF;
        while (SPI2STATbits.SPIBUSY) ;
        if ((R1 = SPI2BUF) != 0xFF)
            break;
    }
    if (R1 != 0x00)
        return (SD_WRITE_ERROR_WRONG_R1);

    SPI2BUF = 0xFF;     // wait one byte
    while (SPI2STATbits.SPIBUSY) ;
    read8 = SPI2BUF;
    
    SPI2BUF = 0b11111110;   // send start token
    while (SPI2STATbits.SPIBUSY) ;
    read8 = SPI2BUF;
    while (i < SD_BLOCK_SIZE)
    {
        SPI2BUF = SD_write_buf[i++];
        while (SPI2STATbits.SPIBUSY) ;
        read8 = SPI2BUF;
    }
    
    retries = SD_RETRIES;
    while (retries--)
    {
        SPI2BUF = 0xFF;
        while (SPI2STATbits.SPIBUSY) ;
        if ((data_response = SPI2BUF) != 0xFF)
            break;
    }
    if ((data_response & 0x10) & !(data_response & 1))
        return (SD_WRITE_ERROR_WRONG_DATA_RESPONSE);

    response_status = (data_response >> 1) & 0b111;
    if (response_status != 0b010)
        return (response_status);

    read8 = 0x00;
    retries = 0;
    while (read8 == 0x00)
    {
        SPI2BUF = 0xFF;
        while (SPI2STATbits.SPIBUSY) ;
        read8 = SPI2BUF;
        retries++;
    }
//    SPI2BUF = 0xFF;     // first crc byte
//    while (SPI2STATbits.SPIBUSY) ;
//    read8 = SPI2BUF;
//    SPI2BUF = 0xFF;     // second crc byte
//    while (SPI2STATbits.SPIBUSY) ;
//    read8 = SPI2BUF;


    SS_MCP_ENCODERS = 0x1;
    SPI2BUF = 0xFF;
    while (SPI2STATbits.SPIBUSY) ;
    read8 = SPI2BUF;
    return (SD_WRITE_NO_ERROR);
}