#include <xc.h>
#include <sys/attribs.h>
#include "0x2a002a.h"

extern struct s_all all;

void	sequencer_pause(void)
{
	int i;

	all.playing = MUSIC_PAUSE;
	for (i = 0; i < INSTRUMENTS_COUNT; i++)
		midi_control_change(i, MCMM_ALL_NOTES_OFF, 0x00);
}

void	sequencer_play(void)
{
	all.playing = MUSIC_PLAYING;
}

void	qtime_generate_note_off(u8 instrument, u8 last_qt[][ATTRIBUTES_PER_NOTE], u8 new_qt[][ATTRIBUTES_PER_NOTE])
{
	u8	i;
	u8	j;
	u8	still;
	u8	old_n;
	u8	new_n;

	for (i = 0; i < NOTES_PER_QTIME; i++)
	{
		old_n = last_qt[i][E_NOTE_VALUE];
		if (old_n == NO_NOTE)
			break;
		still = 0;
		for (j = 0; j < NOTES_PER_QTIME; j++)
		{
			new_n = new_qt[j][E_NOTE_VALUE];
			if (new_n == NO_NOTE)
				break;
			if (NOTE_VALUE(new_n) == NOTE_VALUE(old_n))
			{
				if (IS_NOTE_CONTINUOUS(new_n))
					still = 1;
				break;
			}
		}
		if (!still)
			midi_note_off(instrument, NOTE_VALUE(old_n), last_qt[i][E_NOTE_VELOCITY]);
	}
}

void	qtime_generate_note_on(u8 instrument, u8 qt[][ATTRIBUTES_PER_NOTE])
{
	u8	i;
	u8	*new_n;

	for (i = 0; i < NOTES_PER_QTIME; i++)
	{
		new_n = qt[i];
		if (new_n[E_NOTE_VALUE] == NO_NOTE)
			break;
		else if (IS_NOTE_ATTACK(new_n[E_NOTE_VALUE]))
			midi_note_on(instrument, NOTE_VALUE(new_n[E_NOTE_VALUE]), new_n[E_NOTE_VELOCITY]);
	}
}

void	timer_2_init(void)
{
	TIMER2_STOP_AND_RESET;
	TIMER2_VALUE = 0;
	TIMER2_PERIOD = (FREQUENCY * 15) / (256 /100 * INITIAL_BPM_x100);
	TIMER2_PRESCALE = TIMER_B_PRESCALE_256;
}

void	int_init_timer2(void)
{
	TIMER2_INT_FLAG_CLR;
	TIMER2_INT_PRIORITY = 7;
	TIMER2_INT_ENABLE = INT_ENABLED;
}

void	__ISR(_TIMER_2_VECTOR, IPL7AUTO) Timer2QTime(void)
{
	TIMER2_INT_FLAG_CLR;

	u8	i;

	display_LEDs();
	if (all.playing == MUSIC_PLAYING)
	{
		for (i = 0; i < INSTRUMENTS_COUNT; i++)
		{
			if (all.active_instruments_u16 & (1 << i))
			{
				if (i == all.cur_instrument)
					qtime_generate_note_off(all.cur_instrument, all.cur_active_pattern[(all.qtime - 1) & 0xF], all.cur_active_pattern[all.qtime]);
				else
					qtime_generate_note_off(i, all.active_patterns_array[i][(all.qtime - 1) & 0xF], all.active_patterns_array[i][all.qtime]);
			}
		}
		for (i = 0; i < INSTRUMENTS_COUNT; i++)
		{
			if (all.active_instruments_u16 & (1 << i))
			{
				if (i == all.cur_instrument)
					qtime_generate_note_on(all.cur_instrument, all.cur_active_pattern[all.qtime]);
				else
					qtime_generate_note_on(i, all.active_patterns_array[i][all.qtime]);
			}
		}

		if (all.cur_mode == E_MODE_KEYBOARD)
			update_leds_base_case();
		all.qtime = (all.qtime + 1) & 15;
	}
}
