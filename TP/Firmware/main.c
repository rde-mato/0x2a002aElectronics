#include <xc.h>
#include "tp.h"
#include <sys/attribs.h>

extern u8 I2C2_state;

extern u8   display_buf_dirty;
extern u8   key_scan_dirty;


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
        if (key_scan_dirty && I2C2_READY) // changer les dirty et compgnie vers un byte de status masque.
        {
            read_key_scan();
            execute_changes_in_button_status();
        }
        else if (display_buf_dirty && I2C2_READY)
            led_refresh();
        WDTCONbits.WDTCLR = 1; // CLEAR WATCHDOG
    }
    return (0);
}
