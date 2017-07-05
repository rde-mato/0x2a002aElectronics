#include <xc.h>
#include <sys/attribs.h>
#include "0x2a002a.h"

u8              qtime = 0;
extern u32      leds_status;
extern u8	cur_note;
extern u8          current_mode;
extern u32         leds_active;

extern u8 pattern_mode;

void __ISR(_TIMER_2_VECTOR, IPL3AUTO) Timer2Handler(void)
{
    static u32 last_display = 0;
    u8 i;
    u32 new_display;
    u32 to_toggle;

    TIMER2_INT_FLAG_CLR;


    if (leds_status & (1 << qtime))
    {
        UART1_send(0x90);
        UART1_send(cur_note);
        UART1_send(0x0F);
        UART1_send(0x80);
        UART1_send(cur_note);
        UART1_send(0x0F);
    }
    if (current_mode == E_MODE_DEFAULT)
    {
        new_display = leds_active ^ (1 << qtime);
        to_toggle = last_display ^ new_display;
        i = 0;
        while (i < 16)
        {
            if (to_toggle & (1 << i))
                led_toggle(i);
            ++i;
        }
        last_display = new_display;
    }
    qtime = (qtime + 1) & 15;
}