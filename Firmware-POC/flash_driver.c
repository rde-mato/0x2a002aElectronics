#include <xc.h>
#include <sys/attribs.h>
#include "0x2a002a.h"

#define FLASH_MAX_ADDRESS 0x3ffff

//void    toto(void)
//{
//    u8  read;
//
//
//        SS_FLASH = 0;
//        SPI2BUF = 0x50;
//        while (!SPI2STATbits.SPIRBF) ;
//        read = SPI2BUF;
//        SS_FLASH = 1;
//
//        SS_FLASH = 0;
//        SPI2BUF = 0x01;
//        while (!SPI2STATbits.SPIRBF) ;
//        read = SPI2BUF;
//        SPI2BUF = 0b00000000;
//        while (!SPI2STATbits.SPIRBF) ;
//        read = SPI2BUF;
//        SS_FLASH = 1;
//
//
//        SS_FLASH = 0;
//        SPI2BUF = 0x06;
//        while (!SPI2STATbits.SPIRBF) ;
//        read = SPI2BUF;
//        SS_FLASH = 1;
//
//
//        SS_FLASH = 0;
//        SPI2BUF = 0x05;
//        while (!SPI2STATbits.SPIRBF) ;
//        read = SPI2BUF;
//        SPI2BUF = 0x00;
//        while (!SPI2STATbits.SPIRBF) ;
//        read = SPI2BUF;
//        SS_FLASH = 1;
//
//
//
//        SS_FLASH = 0;
//        SPI2BUF = 0x02;
//        while (!SPI2STATbits.SPIRBF) ;
//        read = SPI2BUF;
//        SPI2BUF = 0x00;
//        while (!SPI2STATbits.SPIRBF) ;
//        read = SPI2BUF;
//        SPI2BUF = 0x00;
//        while (!SPI2STATbits.SPIRBF) ;
//        read = SPI2BUF;
//        SPI2BUF = 0x02;
//        while (!SPI2STATbits.SPIRBF) ;
//        read = SPI2BUF;
//        SPI2BUF = 42;
//        while (!SPI2STATbits.SPIRBF) ;
//        read = SPI2BUF;
//        SS_FLASH = 1;
//
//        read = 10000;
//        while (read)
//        {
//            read --;
//        }
//
//        SS_FLASH = 1;
//        SS_FLASH = 0;
//        SPI2BUF = 0x03;
//        while (!SPI2STATbits.SPIRBF) ;
//        read = SPI2BUF;
//        SPI2BUF = 0x00;
//        while (!SPI2STATbits.SPIRBF) ;
//        read = SPI2BUF;
//        SPI2BUF = 0x00;
//        while (!SPI2STATbits.SPIRBF) ;
//        read = SPI2BUF;
//        SPI2BUF = 0x02;
//        while (!SPI2STATbits.SPIRBF) ;
//        read = SPI2BUF;
//        SPI2BUF = 0x00;
//        while (!SPI2STATbits.SPIRBF) ;
//        read = SPI2BUF;
//        SS_FLASH = 1;
//}