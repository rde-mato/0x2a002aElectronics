#include <xc.h>
#include <sys/attribs.h>
#include <stdlib.h>
#include "0x2a002a.h"

//#define BPM_TO_QBEAT_PR(bpm) ((FREQUENCY * 15) / (256 * bpm))

u32 bpm_x100 = INITIAL_BPM_x100;
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

//void set_qbeat_pr(u16 pr)
//{
//    __g_qbeat_pr = pr;
//    TIMER2_PERIOD = pr;
//    TIMER2_VALUE = 0; //XXX
//}

//void set_bpm(u16 bpm)
//{
//    __g_qbeat_pr = BPM_TO_QBEAT_PR(bpm);
//    TIMER2_PERIOD = __g_qbeat_pr / MIDI_PPQN;
//    TIMER2_VALUE = 0;
//}

void set_bpm(void)
{
    TIMER2_PERIOD = (FREQUENCY * 15) / (MIDI_PPQN * 256 / 100 * bpm_x100);
    if (TIMER2_VALUE > TIMER2_PERIOD - 2000)
        TIMER2_VALUE = 0;
}

//void set_period(u32 pr)
//{
//    TIMER2_PERIOD = (FREQUENCY * 15) / (MIDI_PPQN * 256 / 100 * bpm_x100);
//    if (TIMER2_VALUE > TIMER2_PERIOD - 2000)
//        TIMER2_VALUE = 0;
//}

void change_bpm(s8 incr, u8 rounded)
{
    bpm_x100 += incr;
    if (bpm_x100 < 200)
        bpm_x100 = 200;
    if (rounded)
        bpm_x100 = bpm_x100 - bpm_x100 % 100;
    set_bpm();
}

void    bpm_new_tap(void)
{
    T3CONbits.ON = 1;
    tap_timers[tap_index++] = TIMER3_VALUE;
    TIMER3_VALUE = 0;
}

void    bpm_new_tap_release(void)
{
    u32     mean_tap;

    if (tap_index == 3)
    {
        mean_tap = (tap_timers[0] + tap_timers[1] + tap_timers[2]) / 3;
        bpm_x100 = (70.3125 * FREQUENCY) / (tap_timers[0] + tap_timers[1] + tap_timers[2]);//(100 * 60 * FREQUENCY / 256) / mean_tap;
        tap_timers[0] = tap_timers[1];
        tap_timers[1] = tap_timers[2];
        tap_index = 2;
        TIMER2_PERIOD = mean_tap / (4 * MIDI_PPQN);
        TIMER2_VALUE = 0;
    }
}