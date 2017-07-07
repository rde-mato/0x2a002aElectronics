#include <xc.h>
#include <sys/attribs.h>
#include <stdlib.h>
#include "0x2a002a.h"

#define BPM_TO_QBEAT_PR(bpm) ((FREQUENCY * 15) / (256 * bpm))

u16 __g_qbeat_pr = 0;
u16 tap_button_old = 0;
u16 tap_button_active = 0;
u32 tap_timers[3] = { 0 };
u8  tap_index = 0;



void __ISR(_TIMER_3_VECTOR, IPL1AUTO) Timer3TapButton(void)
{
    T3CONbits.ON = 0;
    TIMER3_INT_FLAG_CLR;
    tap_index = 0;
}

void    int_init_timer3(void)
{
    TIMER3_INT_FLAG_CLR;
    TIMER3_INT_PRIORITY = 1;
    TIMER3_INT_ENABLE = INT_ENABLED;
}

void    timer_3_init(void)
{
    TIMER3_STOP_AND_RESET;
    TIMER3_VALUE = 0;
    TIMER3_PRESCALE = TIMER_PRESCALE_256;
    TIMER3_PERIOD = 0xFFFF;
}

void set_qbeat_pr(u16 pr)
{
    __g_qbeat_pr = pr;
    TIMER2_PERIOD = pr;
    TIMER2_VALUE = 0; //XXX
}

void set_bpm(u16 bpm)
{
    __g_qbeat_pr = BPM_TO_QBEAT_PR(bpm);
    TIMER2_PERIOD = __g_qbeat_pr;
    TIMER2_VALUE = 0;
}

void change_bpm(float incr)
{
    u16 new_pr;
    u32 a;
    u32 b;

    a = (FREQUENCY / 256 * 15);
    b = (((FREQUENCY / 256) * 15 / __g_qbeat_pr) + incr);
    new_pr = (a / b);
    if (incr > 0 && new_pr > __g_qbeat_pr)
        set_qbeat_pr(0x0001);
    else if (incr < 0 && new_pr < __g_qbeat_pr)
        set_qbeat_pr(0xFFFF);
    else
        set_qbeat_pr(new_pr);
}

void    bpm_new_tap(void)
{
    T3CONbits.ON = 1;
    tap_timers[tap_index++] = TIMER3_VALUE;
    TIMER3_VALUE = 0;
}

void    bpm_new_tap_release(void)
{
    u32 new_bpm = 0;

    if (tap_index == 3)
    {
        new_bpm = (tap_timers[0] + tap_timers[1] + tap_timers[2]) / (3 * 4); // pourquoi 4 ?? parce que l'on tape sur un beat pas un qbeat.
        tap_timers[0] = tap_timers[1];
        tap_timers[1] = tap_timers[2];
        tap_index = 2;
        set_qbeat_pr(new_bpm);
    }
}