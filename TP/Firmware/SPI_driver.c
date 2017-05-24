#include <xc.h>
#include <sys/attribs.h>
#include "0x2a002a.h"

u32 SPI2_transmit32bits(u32 msg) //rajouter en argument le SS du chip quand on aura plusieur
{
    u32 res;

    SPI2CONbits.MODE32 = 1;
    LATGbits.LATG9 = 0x0; //SS a 0
    SPI2BUF = msg;
    while (!SPI2STATbits.SPIRBF) ;
    res = SPI2BUF;
    LATGbits.LATG9 = 0x1; //SS a 1
    return (res);
}

u8 SPI2_transmit24bits(u8 part1, u8 part2, u8 part3) //rajouter en argument le SS du chip quand on aura plusieur
{
    u8 res;

    SPI2CONbits.MODE32 = 0;
    LATGbits.LATG9 = 0x0; //SS a 0
    SPI2BUF = part1;
    while (!SPI2STATbits.SPIRBF) ;
    res = SPI2BUF;
    SPI2BUF = part2;
    while (!SPI2STATbits.SPIRBF) ;
    res = SPI2BUF;
    SPI2BUF = part3;
    while (!SPI2STATbits.SPIRBF) ;
    res = SPI2BUF;
    LATGbits.LATG9 = 0x1; //SS a 1
    return (res);
}