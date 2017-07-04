#include <xc.h>
#include <sys/attribs.h>
#include "0x2a002a.h"

u8 eeprom_read_byte = 0;

void eeprom_init()
{
    CS_EEPROM = 0;



    CS_EEPROM = 1;
}

void eeprom_write(u8 byte)
{
    u16 read;

    CS_EEPROM = 0;

    SPI1BUF = 0x06;
    while (SPI1STATbits.SPIBUSY);
    eeprom_read_byte = SPI1BUF;

    CS_EEPROM = 1;

    u16 h = 2000;
    while (h --) ;

    CS_EEPROM = 0;

    SPI1BUF = 0x02;
    while (SPI1STATbits.SPIBUSY);
    eeprom_read_byte = SPI1BUF;

    SPI1CONbits.MODE16 = 1;
    SPI1BUF = 0x0000;
    while (SPI1STATbits.SPIBUSY);
    read = SPI1BUF;
    SPI1CONbits.MODE16 = 0;

    SPI1BUF = byte;
    while (SPI1STATbits.SPIBUSY);
    read = SPI1BUF;

    CS_EEPROM = 1;
}

void eeprom_read(u16 add)
{
    u16 read;

    CS_EEPROM = 0;

    SPI1BUF = 0x03;
    while (SPI1STATbits.SPIBUSY);
    eeprom_read_byte = SPI1BUF;

    SPI1CONbits.MODE16 = 1;
    SPI1BUF = add;
    while (SPI1STATbits.SPIBUSY);
    read = SPI1BUF;
    SPI1CONbits.MODE16 = 0;

    SPI1BUF = 0x00;
    while (SPI1STATbits.SPIBUSY);
    eeprom_read_byte = SPI1BUF;

    CS_EEPROM = 1;
}