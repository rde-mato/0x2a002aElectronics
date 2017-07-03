#include <xc.h>
#include "0x2a002a.h"

u32     leds_status;

u8      HT16_write_leds_request = 1;

void led_set(u8 index)
{
    leds_status |= (1 << index);
    HT16_write_leds_request = 1;
}

void led_toggle(u8 index)
{
    leds_status ^= (1 << index);
    HT16_write_leds_request = 1;
}

void led_clr(u8 index)
{
    leds_status &= ~(1 << index);
    HT16_write_leds_request = 1;
}


void led_refresh(void)
{
    u8	i = 0;
    u8  buffer[8] = { 0 };
    u32 leds = leds_status;

    while (i < 4)
    {
        buffer[2 * i] = (leds & 0x1F) << 3;
        buffer[2 * i + 1] = (leds & 0xE0) >> 5;
        leds = leds >> 8;
        i++;
    }
    I2C1_write(0xE0, 0x08, buffer, 8);
    HT16_write_leds_request = 0;
}