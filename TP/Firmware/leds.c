#include <xc.h>
#include "tp.h"

char pattern[16][4][3];
u8   db_dirty = 1;

u16   displaybuff[4];
u8   buttonbuff[16];

static u16  ledmatrix[16] = {
    0x0004, 0x8000, 0x2000, 0x1000,
    0x0001, 0x0002, 0x0800, 0x1000, /// DUPLICAT ?!
    0x4000, 0x0008, 0x0200, 0x0100,
    0x0040, 0x0020, 0x0010, 0x0400,
};
//ks1 4: 0x0400 11: 0x0001 12: 0x0200 16: 0x0002

//ks0 1: 0x8000 2: 0x1000 3: 0x0400 5: 0x2000 6: 0x4000 7: 0x0100 8: 0x0200 9: 0x0800 10: 0x0001 13: 0x0008 14: 0x0004 15: 0x0002


const u32 buttonmatrix[16] = {
    0x80000000,
    0x10000000,
    0x04000000,
    0x00000400,
    0x20000000,
    0x40000000,
    0x01000000,
    0x02000000,
    0x08000000,
    0x00010000,
    0x00000001,
    0x00000200,
    0x00080000,
    0x00040000,
    0x00020000,
    0x00000002
};



void led_set(u8 index)
{
    displaybuff[3 - index / 4] |= ledmatrix[index];
    db_dirty = 1;
}

void led_toggle(u8 index)
{
    displaybuff[3 - index / 4] ^= ledmatrix[index];
    db_dirty = 1;
}

void led_clr(u8 index)
{
    displaybuff[3 - index / 4] &= ~ledmatrix[index];
    db_dirty = 1;
}

void led_refresh(void)
{
    int	i;

    I2C2_push(0xE0);
    I2C2_push(0x00);
    i = 0;
    while (i < 4)
    {
        I2C2_push(displaybuff[i] >> 8);
        I2C2_push(displaybuff[i] & 0xFF);
        i++;
    }
    I2C2_write();
    db_dirty = 0;
}