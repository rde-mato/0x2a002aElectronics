#include <xc.h>
#include <sys/attribs.h>
#include "0x2a002a.h"

u8              qtime = 0;
extern u32      leds_status;
extern u8 cpt;

extern u8 pattern_mode;

void __ISR(_TIMER_2_VECTOR, IPL3AUTO) Timer2Handler(void)
{
    TIMER2_INT_FLAG_CLR;
    if (leds_status & (1 << qtime))
    {
        UART1_send(0x90);
        UART1_send(cpt);
        UART1_send(0x0F);
        UART1_send(0x80);
        UART1_send(cpt);
        UART1_send(0x0F);
    }
    if (pattern_mode)
    {
        if (leds_status & (1 << qtime))
            led_clr(qtime);
        else
            led_set(qtime);
        led_toggle((qtime - 1) & 15);
    }
    qtime = (qtime + 1) & 15;
}