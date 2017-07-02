#include <xc.h>
#include <sys/attribs.h>
#include "0x2a002a.h"

extern  u8  HT16_write_leds_request;
extern  u8  HT16_read_keys_request;
extern  u8  I2C1_state;

void    manage_I2C1(void)
{
    if (!I2C1_READY)
        return ;
    if (HT16_write_leds_request)
        led_refresh();
    else if (HT16_read_keys_request)
         key_scan();
}