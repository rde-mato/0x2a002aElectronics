#include <xc.h>
#include <sys/attribs.h>
#include "0x2a002a.h"


u8      HT16_read_request = 0;
u8      button_buf[16];
u32     current_key_scan = 0;
u32     previous_key_scan = 0;

u8      event_type = 0;
u8      event_source = 0;

u32     poll_count = 0;
u32     buttons_timers[32] = {0};

extern u8  I2C2_read_request;
extern  u8 I2C2_read_buf[READ_BUF_SIZE];

const u32 buttonmatrix[16] = {
    0x80000000, 0x10000000, 0x04000000, 0x00000400,
    0x20000000, 0x40000000, 0x01000000, 0x02000000,
    0x08000000, 0x00010000, 0x00000001, 0x00000200,
    0x00080000, 0x00040000, 0x00020000, 0x00000002
};


void __ISR(_EXTERNAL_2_VECTOR, IPL2AUTO) HT16IntHandler(void) {
    HT16_read_request = 1;
    TMR4 = 0;
    ++poll_count;
    T4CONbits.ON = 1;
    IFS0bits.INT2IF = 0;
}

void __ISR(_TIMER_5_VECTOR, IPL2AUTO) Timer5Handler(void)
{
    IFS0bits.T5IF = 0;
    T4CONbits.ON = 0;
    current_key_scan = 0;
    HT16_read_request = 1;
}

void send_button_interrupt(u8 interrupt_type, u8 interrupt_button)
{
    event_type = interrupt_type;
    event_source = interrupt_button;
    IFS0bits.CS0IF = 1;
}

void process_key_scan(void)
{
    u32 changed_buttons;
    u32 unchanged_pressed_buttons;
    u32 current_poll_count;
    u32 newly_pressed_buttons;
    u32 newly_released_buttons;
    u8  i;

    current_poll_count = poll_count;
    poll_count = 0;
    changed_buttons = current_key_scan ^ previous_key_scan;
    unchanged_pressed_buttons = current_key_scan & previous_key_scan;
    newly_pressed_buttons = current_key_scan & ~previous_key_scan;
    newly_released_buttons = ~current_key_scan & previous_key_scan;

    if (newly_pressed_buttons)
    {
        i = 0;
        while (i < 32)
        {
            if (newly_pressed_buttons & (1 << i))
                send_button_interrupt(E_KEY_PRESSED, i);
            ++i;
        }
    }
    if (newly_released_buttons)
    {
        i = 0;
        while (i < 32)
        {
            if (newly_released_buttons & (1 << i))
                send_button_interrupt(E_KEY_RELEASED, i);
            ++i;
        }
    }
    if (unchanged_pressed_buttons)
    {
        i = 0;
        while (i < 32)
        {
            if (unchanged_pressed_buttons & (1 << i))
            {
                buttons_timers[i] += current_poll_count;
                if(buttons_timers[i] > LONG_PRESS_LIMIT)
                {
                    send_button_interrupt(E_KEY_LONG_PRESSED, i);
                    buttons_timers[i] = 0;
                }
            }
            else
                buttons_timers[i] = 0;
            ++i;
        }
    }
    else
    {
        while (i < 32)
            buttons_timers[i++] = 0;
    }
    previous_key_scan = current_key_scan;
}

void key_scan(void)
{
    I2C2_read_request = E_KEYSCAN;
    HT16_read_request = 0;
    I2C2_push(0xE0);
    I2C2_push(0x40);
    I2C2_read(6);
}

void format_key_scan(void)
{
    u32 ks;
    u8 i;

    ks = (I2C2_read_buf[0] << 24 )| (I2C2_read_buf[1] << 16) | (I2C2_read_buf[2] << 8) | I2C2_read_buf[3] ;
    i = 0;
    current_key_scan = 0;
    while (i < 16)
    {
        if (buttonmatrix[i] & ks)
            current_key_scan |= (1 << (i));
        ++i;
    }
}
