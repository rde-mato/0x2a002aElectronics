#include <xc.h>
#include <sys/attribs.h>
#include "0x2a002a.h"

#define FLASH_MAX_ADDRESS 0x3ffff

//void    toto(void)
//{
//    SS_FLASH = 1;
//        SS_FLASH = 0;
//        SPI2BUF = 0x05;
//        while (!SPI2STATbits.SPIRBF) ;
//        read = SPI2BUF;
//        SPI2BUF = 0x00;
//        while (!SPI2STATbits.SPIRBF) ;
//        read = SPI2BUF;
//        if (!(read & 0x01))
//        {
//            ;
//        }
//        SS_FLASH = 1;
//}