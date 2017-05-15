#include <xc.h>
#include <sys/attribs.h>
#include "0x2a002a.h"

u16 spirx;

void SPI2_write(void)
{
    LATGbits.LATG9 = 0x0;
    SPI2BUF = 0x4012;
    while (SPI2STATbits.SPIBUSY) ;
    LATGbits.LATG9 = 0x1;
    SPI2BUF = 0x4112;
    LATGbits.LATG9 = 0x0;
    spirx = SPI2BUF;
    while (SPI2STATbits.SPIBUSY) ;
    LATGbits.LATG9 = 0x1;
}