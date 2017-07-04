#include <xc.h>
#include <sys/attribs.h>
#include "0x2a002a.h"

u32 SD_return;
u8  SD_error = 0;
u8  SD_type = SD_SDSC;

u8  SD_read_buf[SD_BLOCK_SIZE] = {0};
u8  SD_write_buf[SD_BLOCK_SIZE] = "coucou c'est moi";

void    send_lots_clk(void)
{
    u32 read32;
    u8  i = 20;

    SPI1CONbits.MODE32 = 1;
    while (--i)
    {
        SPI1BUF = 0xFFFFFFFF;
        while (SPI1STATbits.SPIBUSY) ;
        read32 = SPI1BUF;
    }
    SPI1CONbits.MODE32 = 0;
}

//u8  sd_command(u8 cmd, u32 arg, u8 crc, u8 restype)
//{
//    u8  read8;
//    u32 read32;
//    u8  ret;
//
//    SS_MCP_ENCODERS = 0x0;
//    SPI1BUF = 0x40 | cmd;
//    while (SPI1STATbits.SPIBUSY) ;
//    read8 = SPI1BUF;
//
//    SPI1CONbits.MODE32 = 1;
//
//    SPI1BUF = arg;
//    while (SPI1STATbits.SPIBUSY) ;
//    read32 = SPI1BUF;
//
//    SPI1CONbits.MODE32 = 0;
//
//    SPI1BUF = crc;
//    while (SPI1STATbits.SPIBUSY) ;
//    read8 = SPI1BUF;
//
//    SPI1BUF = 0xFF;
//    while (SPI1STATbits.SPIBUSY) ;
//    ret = SPI1BUF;
//
//    if (restype == SD_R3 || restype == SD_R7)
//    {
//        SPI1CONbits.MODE32 = 1;
//        SPI1BUF = 0xFFFFFFFF;
//        while (SPI1STATbits.SPIBUSY) ;
//        SD_return = SPI1BUF;
//        SPI1CONbits.MODE32 = 0;
//    }
//    SS_MCP_ENCODERS = 0x1;
//    SPI1BUF = 0xFF;
//    while (SPI1STATbits.SPIBUSY) ;
//    read8 = SPI1BUF;
//    return (ret);
//}

u8  SD_command_retry(u8 cmd, u32 arg, u8 crc, u8 restype)
{
    u8  read8;
    u32 read32;
    u8  ret = 0xFF;
    u32  retries = SD_RETRIES;

    CS_SD = 0x0;
    SPI1BUF = 0x40 | cmd;
    while (SPI1STATbits.SPIBUSY) ;
    read8 = SPI1BUF;

    SPI1CONbits.MODE32 = 1;

    SPI1BUF = arg;
    while (SPI1STATbits.SPIBUSY) ;
    read32 = SPI1BUF;

    SPI1CONbits.MODE32 = 0;

    SPI1BUF = crc;
    while (SPI1STATbits.SPIBUSY) ;
    read8 = SPI1BUF;

    while (retries--)
    {
        SPI1BUF = 0xFF;
        while (SPI1STATbits.SPIBUSY) ;
        if ((ret = SPI1BUF) != 0xFF)
            break;
    }
    if (ret != 0xFF && (restype == SD_R3 || restype == SD_R7))
    {
        SPI1CONbits.MODE32 = 1;
        SPI1BUF = 0xFFFFFFFF;
        while (SPI1STATbits.SPIBUSY) ;
        SD_return = SPI1BUF;
        SPI1CONbits.MODE32 = 0;
    }

    CS_SD = 0x1;
    SPI1BUF = 0xFF;
    while (SPI1STATbits.SPIBUSY) ;
    read8 = SPI1BUF;
    return (ret);
}

void SD_card_init(void)
{
    u8  R1_response;
    u32  retries = SD_RETRIES;

    // baisser le baud rate du spi
//    SPI1BRG = 9; //divise la frequence de SCK par 20 = 2 * (9 + 1)
    SPI1BRG = 3; //divise la frequence de SCK par 6 = 2 * (2 + 1)
    send_lots_clk();
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
    SPI1BRG = 0; //set baudrate 1Mhz suivant 8 Mhz du pbclk
}

u8  SD_card_read_block(u32 block)
{
    u8  R1;
    u8  token;
    u8  read8;
    u32 read32;
    u32  retries = SD_RETRIES;
    u16 i = 0;

    CS_SD = 0x0;
    SPI1BUF = 0x40 | 17;
    while (SPI1STATbits.SPIBUSY) ;
    read8 = SPI1BUF;

    SPI1CONbits.MODE32 = 1;

    if (SD_type == SD_SDSC)
        block *= SD_BLOCK_SIZE;
    SPI1BUF = block;
    while (SPI1STATbits.SPIBUSY) ;
    read32 = SPI1BUF;

    SPI1CONbits.MODE32 = 0;

    SPI1BUF = 0x01;
    while (SPI1STATbits.SPIBUSY) ;
    read8 = SPI1BUF;

    while (retries--)
    {
        SPI1BUF = 0xFF;
        while (SPI1STATbits.SPIBUSY) ;
        if ((R1 = SPI1BUF) != 0xFF)
            break;
    }
    if (R1 != 0x00)
        return (SD_READ_ERROR_WRONG_R1);

    retries = SD_RETRIES;
    while (retries--)
    {
        SPI1BUF = 0xFF;
        while (SPI1STATbits.SPIBUSY) ;
        if ((token = SPI1BUF) == 0b11111110)
            break;
    }
    if (token != 0b11111110)
        return (SD_READ_ERROR_WRONG_TOKEN);
    while (i < SD_BLOCK_SIZE)
    {
        SPI1BUF = 0xFF;
        while (SPI1STATbits.SPIBUSY) ;
        SD_read_buf[i++] = SPI1BUF;
    }
    SPI1BUF = 0xFF;
    while (SPI1STATbits.SPIBUSY) ;
    read8 = SPI1BUF;
    SPI1BUF = 0xFF;
    while (SPI1STATbits.SPIBUSY) ;
    read8 = SPI1BUF;

    CS_SD = 0x1;
    SPI1BUF = 0xFF;
    while (SPI1STATbits.SPIBUSY) ;
    read8 = SPI1BUF;
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

    CS_SD = 0x0;
    SPI1BUF = 0x40 | 24;
    while (SPI1STATbits.SPIBUSY) ;
    read8 = SPI1BUF;

    SPI1CONbits.MODE32 = 1;

    if (SD_type == SD_SDSC)
        block *= SD_BLOCK_SIZE;
    SPI1BUF = block;
    while (SPI1STATbits.SPIBUSY) ;
    read32 = SPI1BUF;

    SPI1CONbits.MODE32 = 0;

    SPI1BUF = 0x01;
    while (SPI1STATbits.SPIBUSY) ;
    read8 = SPI1BUF;

    while (retries--)
    {
        SPI1BUF = 0xFF;
        while (SPI1STATbits.SPIBUSY) ;
        if ((R1 = SPI1BUF) != 0xFF)
            break;
    }
    if (R1 != 0x00)
        return (SD_WRITE_ERROR_WRONG_R1);

    SPI1BUF = 0xFF;     // wait one byte
    while (SPI1STATbits.SPIBUSY) ;
    read8 = SPI1BUF;
    
    SPI1BUF = 0b11111110;   // send start token
    while (SPI1STATbits.SPIBUSY) ;
    read8 = SPI1BUF;
    while (i < SD_BLOCK_SIZE)
    {
        SPI1BUF = SD_write_buf[i++];
        while (SPI1STATbits.SPIBUSY) ;
        read8 = SPI1BUF;
    }
    
    retries = SD_RETRIES;
    while (retries--)
    {
        SPI1BUF = 0xFF;
        while (SPI1STATbits.SPIBUSY) ;
        if ((data_response = SPI1BUF) != 0xFF)
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
        SPI1BUF = 0xFF;
        while (SPI1STATbits.SPIBUSY) ;
        read8 = SPI1BUF;
        retries++;
    }
//    SPI1BUF = 0xFF;     // first crc byte
//    while (SPI1STATbits.SPIBUSY) ;
//    read8 = SPI1BUF;
//    SPI1BUF = 0xFF;     // second crc byte
//    while (SPI1STATbits.SPIBUSY) ;
//    read8 = SPI1BUF;


    CS_SD = 0x1;
    SPI1BUF = 0xFF;
    while (SPI1STATbits.SPIBUSY) ;
    read8 = SPI1BUF;
    return (SD_WRITE_NO_ERROR);
}