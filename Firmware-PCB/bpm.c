#include <xc.h>
#include <sys/attribs.h>
#include <stdlib.h>
#include "0x2a002a.h"

extern struct s_all	all;
static u32	tap_timers[3] = { 0 };
static u8	tap_index = 0;

void	__ISR(_TIMER_3_VECTOR, IPL1AUTO) Timer3TapButton(void)
{
	T3CONbits.ON = 0;
	TIMER3_VALUE = 0;
	TIMER3_INT_FLAG_CLR;
	tap_index = 0;
}

void	int_init_timer3(void)
{
	TIMER3_INT_FLAG_CLR;
	TIMER3_INT_PRIORITY = 1;
	TIMER3_INT_ENABLE = INT_ENABLED;
}

void	timer_3_init(void)
{
	TIMER3_STOP_AND_RESET;
	TIMER3_VALUE = 0;
	TIMER3_PRESCALE = TIMER_B_PRESCALE_256;
	TIMER3_PERIOD = 0xFFFF;
}

void	set_bpm(void)
{
	TIMER2_PERIOD = (FREQUENCY * 15) / (256 * all.bpm_x100 / 100);
	TIMER2_VALUE = 0;
}

void	change_bpm(s8 incr, u8 rounded)
{
	all.bpm_x100 += incr;
	if (all.bpm_x100 < 200)
		all.bpm_x100 = 200;
	if (rounded)
		all.bpm_x100 = all.bpm_x100 - all.bpm_x100 % 100;
	set_bpm();
}

inline void	bpm_reset_tap(void)
{
	tap_index = 0;
}

void	bpm_new_tap(void)
{
	if (T3CONbits.ON)
		tap_timers[tap_index++] = TIMER3_VALUE;
	else
		T3CONbits.ON = 1;
	TIMER3_VALUE = 0;
}

void	bpm_new_tap_release(void)
{
	u32	mean_tap;

	if (tap_index == 3 && T3CONbits.ON)
	{
		mean_tap = (tap_timers[0] + tap_timers[1] + tap_timers[2]) / 3;
		all.bpm_x100 = (70.3125 * FREQUENCY) / (tap_timers[0] + tap_timers[1] + tap_timers[2]);//(100 * 60 * FREQUENCY / 256) / mean_tap;
		tap_timers[0] = tap_timers[1];
		tap_timers[1] = tap_timers[2];
		tap_index = 2;
		// TIMER2_PERIOD = mean_tap / (4 * MIDI_PPQN);
		// TIMER2_VALUE = 0;
		set_bpm();
	}
}
