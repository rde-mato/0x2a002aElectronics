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
    I2C2_init();
    INT_init();
    T2CONbits.ON = 1;

    HT16_init();

    while (42)
    {
        manage_I2C2();
        WDTCONbits.WDTCLR = 1; // CLEAR WATCHDOG
    }
    return (0);
}
