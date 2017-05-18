#include <xc.h>
#include <sys/attribs.h>
#include "0x2a002a.h"

u32 SPI2_transmit32bits(u32 msg) //rajouter en argument le SS du chip quand on aura plusieur
{
    u32 res;

    LATGbits.LATG9 = 0x0; //SS a 0
    SPI2BUF = msg;
    while (!SPI2STATbits.SPIRBF) ;
    res = SPI2BUF;
    LATGbits.LATG9 = 0x1; //SS a 1
    return (res);
}