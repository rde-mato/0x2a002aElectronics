#include <xc.h>
#include "tp.h"
#include <sys/attribs.h>

u8  key_scan_dirty = 0;
u16 button_buf[16] = {0};
u32 new_button_status = 0;            // a mettre a jour quand on aura plus de bouttons
u32 old_button_status = 0;            // a mettre a jour quand on aura plus de bouttons

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
extern  u8 I2C2_read_buf[READ_BUF_SIZE];
//extern  u32  leds_status;

//ca passe ici a chaque boutons pressed
void __ISR(_EXTERNAL_2_VECTOR, IPL2AUTO) Int2Handler(void) {
    key_scan_dirty = 1;
    TMR4 = 0;
    T4CONbits.ON = 1;
    IFS0bits.INT2IF = 0; // Reset the flag
}


//ca passe la quand tous les boutons sont released
void __ISR(_TIMER_5_VECTOR, IPL2AUTO) Timer5Handler(void)
{
//    LED_ON_OFF = !LED_ON_OFF;

    key_scan_dirty = 1;
    IFS0bits.T5IF = 0;
    T4CONbits.ON = 0;
}

void read_key_scan(void)
{
    I2C2_push(0xE0);
    I2C2_push(0x40);
    I2C2_read(6);
    key_scan_dirty = 0;
    refresh_button_status();     ///// faudrait attendre i2c done nan ?
}

void refresh_button_status(void)
{
    u32 ks;
    u8 i;

    //ks = *((u32*)I2C2_read_buf);
    ks = (I2C2_read_buf[0] << 24 ) | (I2C2_read_buf[1] << 16) | (I2C2_read_buf[2] << 8) | I2C2_read_buf[3] ;
    old_button_status = new_button_status;            // a mettre a jour quand on aura plus de bouttons
    new_button_status = 0;
    i = 0;
    while (i < 16)
    {
        if (buttonmatrix[i] & ks)
        {
            new_button_status |= (1 << (i));
            button_buf[i]++;
            if (button_buf[i] > 100)
            {
                LED_ON_OFF = !LED_ON_OFF;      /////// veut dire, long presse
                button_buf[i] = 0;
            }
        }
        ++i;
    }
}


void execute_changes_in_button_status(void)
{
     u8  i;
     u32 old_button;
     u32 new_button;

    i = 0;
    while (i < 16)
    {
        new_button = new_button_status & (1 << i);
        old_button = old_button_status & (1 << i);
        if (new_button && (new_button != old_button))
            led_toggle(i);                          // a changer apres le test que ca fonctionne bien
        ++i;
    }
}