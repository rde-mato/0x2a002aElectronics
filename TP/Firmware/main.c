#include <xc.h>
#include <sys/attribs.h>
#include "0x2a002a.h"

char pattern[16][4][3];

int main(void)
{
    init_pattern();

    GPIO_init();
    TIMER_init();
    I2C2_init();
    SPI2_init();
    
    INT_init();

    HT16_init();
   // MCP_init();
    MCP_write(0x04, 0xFFFF);
    T2CONbits.ON = 1;

    led_toggle(0);

    while (42)
    {
        manage_I2C2();
        WDTCONbits.WDTCLR = 1; // CLEAR WATCHDOG
    }
    return (0);
}
