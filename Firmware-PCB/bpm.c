#include <xc.h>
#include <sys/attribs.h>
#include <stdlib.h>
#include "0x2a002a.h"

#define BPM_TO_QBEAT_PR(bpm) ((FREQUENCY * 15) / (256 * bpm))

u16 __g_qbeat_pr = 0;
u16 tap_button_old = 0;
u16 tap_button_active = 0;

void __ISR(_TIMER_3_VECTOR, IPL1AUTO) Timer3TapButton(void)
{
    T3CONbits.ON = 0;
    TIMER3_INT_FLAG_CLR;
    tap_button_old = 0;
    tap_button_active = 0;
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

    a = (8000000 / 256 * 15);
    b = (((8000000 / 256) * 15 / __g_qbeat_pr) + incr);
    new_pr = (a / b);
/*    if (incr > 0 && new_pr > __g_qbeat_pr)
        set_qbeat_pr(0x0001);
    else if (incr < 0 && new_pr < __g_qbeat_pr)
        set_qbeat_pr(0xFFFF);
    else*/
        set_qbeat_pr(new_pr);
}

void    bpm_new_tap(void)
{

    u16 tap_button_new = TIMER3_VALUE;
    if (tap_button_active)
    {
        if (tap_button_old)
        {
            set_qbeat_pr(((u32)tap_button_old + tap_button_new) / 8);
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