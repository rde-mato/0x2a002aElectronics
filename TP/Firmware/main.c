#include <xc.h>
#include <sys/attribs.h>
#include "0x2a002a.h"

extern u8 I2C2_state;
char pattern[16][4][3];



int main(void)
{
    init_pattern();

    GPIO_init();
    TIMER_init();
//    I2C2_init();
    SPI2_init();
    
    INT_init();

 //   HT16_init();
    T2CONbits.ON = 1;

    LATGbits.LATG9 = 0;
    u16 rb = SPI2BUF;
    SPI2BUF = 0x4100;
    while (!SPI2STATbits.SPIRBF);
    rb = SPI2BUF;
    LATGbits.LATG9 = 1;
    LATGbits.LATG9 = 0;
    SPI2BUF = 0x4102;
    while (!SPI2STATbits.SPIRBF);
    rb = SPI2BUF;
    int i = 0;
 //   MCP_init();
 //   led_toggle(0);

    while (42)
    {
//        manage_I2C2();
//        manage_SPI2();
        WDTCONbits.WDTCLR = 1; // CLEAR WATCHDOG
    }
    return (0);
}
