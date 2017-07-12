#include <xc.h>
#include "0x2a002a.h"

extern u32     current_leds_on;
u8             HT16_write_leds_request = 1;

void led_set(u8 index)
{
    current_leds_on |= (1 << index);
    HT16_write_leds_request = 1;
}

void led_toggle(u8 index)
{
    current_leds_on ^= (1 << index);
    HT16_write_leds_request = 1;
}

void led_clr(u8 index)
{
    current_leds_on &= ~(1 << index);
    HT16_write_leds_request = 1;
}

void clear_led_request(void)
{
    HT16_write_leds_request = 0;
}

void led_refresh(void)
{
    u8	i = 0;
    u8  buffer[8] = { 0 };
    u32 leds = current_leds_on;

    while (i < 4)
    {
        buffer[2 * i] = (leds & 0x1F) << 3;
        buffer[2 * i + 1] = (leds & 0xE0) >> 5;
        leds = leds >> 8;
        i++;
    }
    I2C1_write_callback(0xE0, 0x08, buffer, 8, NULL);
}