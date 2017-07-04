#include <xc.h>
#include <sys/attribs.h>
#include "0x2a002a.h"

#define BPM_TO_FREQ(bpm) ((FREQUENCY / (bpm / 15)) / 256)

u16 g_bpm;
u16 tap_button_old = 0;
u16 tap_button_active = 0;

void __ISR(_TIMER_3_VECTOR, IPL1AUTO) Timer3Handler(void)
{
    T3CONbits.ON = 0;
    TIMER3_INT_FLAG_CLR;
    tap_button_old = 0;
    tap_button_active = 0;
}

void set_bpm_pr(u16 pr)
{
    u8 buf[5] = {0};

    TIMER2_PERIOD = pr;
    TIMER2_VALUE = 0;
    g_bpm = (FREQUENCY * 15) / (pr * 256);
    lcditoa(g_bpm, buf, 4);
    LCD_putstr(3, 3, buf);
    LCD_print_changed_chars();
}

void set_bpm(u8 bpm)
{
    u8 buf[5] = {0};

    TIMER2_PERIOD = BPM_TO_FREQ(bpm);
    TIMER2_VALUE = 0;
    g_bpm = bpm;
    lcditoa(bpm, buf, 4);
    LCD_putstr(3, 3, buf);
    LCD_print_changed_chars();
}

void    bpm_new_tap(void)
{

    u16 tap_button_new = TIMER3_VALUE;
    if (tap_button_active)
    {
        if (tap_button_old)
        {
            set_bpm_pr(((u32)tap_button_old + tap_button_new) / (2 * 4));
        }
        tap_button_old = tap_button_new;
        TIMER3_VALUE = 0;
    }
    else
    {
        tap_button_active = 1;
        T3CONbits.ON = 1;
    }
}