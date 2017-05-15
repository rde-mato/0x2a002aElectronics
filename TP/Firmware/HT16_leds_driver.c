#include <xc.h>
#include "0x2a002a.h"

u32     leds_status;
u16     display_buf[4];
u8      display_buf_dirty = 1;

static u16  ledmatrix[16] = {
    0x0004, 0x8000, 0x2000, 0x1000,
    0x0001, 0x0002, 0x0800, 0x1000,
    0x4000, 0x0008, 0x0200, 0x0100,
    0x0040, 0x0020, 0x0010, 0x0400,
};


void led_set(u8 index)
{
    //display_buf[3 - index >> 2] |= ledmatrix[index];
    leds_status |= (1 << index);
    display_buf_dirty = 1;
}

void led_toggle(u8 index)
{
    //display_buf[3 - index / 4] ^= ledmatrix[index];
    leds_status ^= (1 << index);
    display_buf_dirty = 1;
}

void led_clr(u8 index)
{
    //display_buf[3 - index / 4] &= ~ledmatrix[index];
    leds_status &= ~(1 << index);
    display_buf_dirty = 1;
}


void clear_display_buffer(void)
{
    u8  i;

    i = 0;
    while (i < 4)
        display_buf[i++] = 0;
}

void leds_status_to_display_buffer(void)
{
    u8  index;

    clear_display_buffer();
    index = 0;
    while (index < 32)
    {
        if (leds_status & (1 << index))
            display_buf[3 - (index >> 2)] |= ledmatrix[index];
        index++;
    }
}

void led_refresh(void)
{
    int	i;

    leds_status_to_display_buffer();
    I2C2_push(0xE0);
    I2C2_push(0x00);
    i = 0;
    while (i < 4)
    {
        I2C2_push(display_buf[i] >> 8);
        I2C2_push(display_buf[i] & 0xFF);
        i++;
    }
    I2C2_write();
    display_buf_dirty = 0;
}

