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
    TIMER3_OFF;
    TIMER3_SET_VALUE(0);
    TIMER3_INT_FLAG_CLR;
    tap_index = 0;
}

void    int_init_timer3(void)
{
    TIMER3_INT_FLAG_CLR;
    TIMER3_INT_PRIORITY = 1;
    TIMER3_SET_INT_ENABLE(INT_ENABLED);
}

void    timer_3_init(void)
{
    TIMER3_STOP_AND_RESET;
    TIMER3_SET_VALUE(0);
    TIMER3_SET_PRESCALE(TIMER_B_PRESCALE_256);
    TIMER3_SET_PERIOD(0xFFFF);
}

void set_bpm(void)
{
    TIMER2_SET_PERIOD((FREQUENCY * 15) / (256 * bpm_x100 / 100));
    TIMER2_SET_VALUE(0);
}

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
    if (TIMER3_IS_ON)
        tap_timers[tap_index++] = TIMER3_GET_VALUE;
    else
      TIMER3_ON;
    TIMER3_SET_VALUE(0);
}

void    bpm_new_tap_release(void)
{
    u32     mean_tap;

    if (tap_index == 3 && TIMER3_IS_ON)
    {
        mean_tap = (tap_timers[0] + tap_timers[1] + tap_timers[2]) / 3;
        bpm_x100 = (70.3125 * FREQUENCY) / (tap_timers[0] + tap_timers[1] + tap_timers[2]);//(100 * 60 * FREQUENCY / 256) / mean_tap;
        tap_timers[0] = tap_timers[1];
        tap_timers[1] = tap_timers[2];
        tap_index = 2;
//        TIMER2_PERIOD = mean_tap / (4 * MIDI_PPQN);
//        TIMER2_VALUE = 0;
        set_bpm();
    }
}
