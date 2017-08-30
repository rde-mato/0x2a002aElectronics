#include <xc.h>
#include <sys/attribs.h>
#include "0x2a002a.h"

extern struct s_all all;

extern u8	HT16_write_leds_request;
static u8	noteskeys[13] = { 8, 1, 9, 2, 10, 11, 4, 12, 5, 13, 6, 14, 15};

void	display_LEDs(void)
{
	u8	i;
	u8	blinking = 0;
	u32	to_display;
	u32	to_toggle;
	u8	to_blink;
	static	u8 blink = 0;

	to_display = all.leds_base_case;
	switch (all.cur_mode)
	{
		case E_MODE_INSTRU:
			blinking = 1;
			to_blink = all.cur_instrument;
			break;
		case E_MODE_EDIT_INSTRU:
			blinking = 1;
			to_blink = all.cur_instrument;
			break;
		case E_MODE_PATTERN:
			if (!(all.playing == MUSIC_PLAYING))
			{
				blinking = 1;
				to_blink = all.qtime;
			}
			else
				to_display ^= (1 << all.qtime);
			break;
		case E_MODE_EDIT_PATTERN:
			blinking = 1;
			to_blink = all.cur_pattern;
			break;
		case E_MODE_KEYBOARD:
			break;
		case E_MODE_EDIT_KEYBOARD:
			break;
	}
	blink = !blink;
	if (blinking && blink == 1)
		to_display ^= (1 << to_blink);
	to_toggle = all.current_leds_on ^ to_display;
	i = 0;
	while (i < 32)
	{
		if (to_toggle & (1 << i))
			led_toggle(i);
		++i;
	}
	all.current_leds_on = to_display;
}

u32	keyboard_get_playing_notes(void)
{
	u8	i;
	u8	j;
	u8	note;
	u32	ret;

	ret = 0;
	for (i = 0; i < NOTES_PER_QTIME; i++)
	{
		note = all.cur_active_pattern[all.qtime][i][E_NOTE_VALUE];
		if (note == NO_NOTE)
			break;
		note = NOTE_VALUE(note);
		if (note / 12 == all.cur_octave)
			ret ^= 1 << (noteskeys[note % 12]);
		else if (note == 12 * (all.cur_octave + 1))
			ret ^= 1 << (E_SOURCE_KEY_15);
	}
	return (ret);
}

void	update_leds_base_case(void)
{
	u8	qt = 0;
	u8	n;
	u8	i;
	u8	p;

	all.leds_base_case = 0;
	if (all.playing == MUSIC_PLAYING)
		all.leds_base_case |= (1 << E_SOURCE_BUTTON_PLAY_PAUSE);
	if (CUE_PRESSED)
		all.leds_base_case |= (1 << E_SOURCE_BUTTON_CUE);
	all.leds_base_case |= (1 << (all.cur_octave + E_SOURCE_ENCODER_0));
	switch (all.cur_mode)
	{
		case E_MODE_INSTRU:
			while (qt < INSTRUMENTS_COUNT)
			{
				if (all.active_instruments_u16 & (1 << qt))
					all.leds_base_case |= (1 << qt);
				qt++;
			}
			all.leds_base_case |= (1 << E_SOURCE_BUTTON_INSTRUMENT);
			display_LEDs();
			break;
		case E_MODE_EDIT_INSTRU:
			while (qt < INSTRUMENTS_COUNT)
			{
				if (all.active_instruments_u16 & (1 << qt))
					all.leds_base_case |= (1 << qt);
				qt++;
			}
			all.leds_base_case |= (1 << E_SOURCE_BUTTON_INSTRUMENT);
			all.leds_base_case |= (1 << E_SOURCE_BUTTON_EDIT);
			display_LEDs();
			break;
		case E_MODE_PATTERN:
			while (qt < QTIME_PER_PATTERN)
			{
				n = 0;
				while (n < NOTES_PER_QTIME)
					if (NOTE_VALUE(all.cur_active_pattern[qt][n++][0]) == all.cur_note)
						all.leds_base_case |= (1 << qt);
				qt++;
			}
			all.leds_base_case |= (1 << E_SOURCE_BUTTON_PATTERN);
			display_LEDs();
			break;
		case E_MODE_EDIT_PATTERN:
			p = 0;
			while (p < PATTERNS_PER_INSTRUMENT)
			{
				qt = 0;
				while (qt < QTIME_PER_PATTERN)
					if (all.active_instrument[p][qt++][0][0] != NO_NOTE)
					{
						all.leds_base_case |= (1 << p);
						break ;
					}
				++p;
			}
			all.leds_base_case |= (1 << E_SOURCE_BUTTON_EDIT);
			all.leds_base_case |= (1 << E_SOURCE_BUTTON_PATTERN);
			display_LEDs();
			break;
		case E_MODE_KEYBOARD:
			all.leds_base_case |= PIANO_KEYS;
			all.leds_base_case |= (1 << E_SOURCE_BUTTON_KEYBOARD);
			all.leds_base_case ^= keyboard_get_playing_notes();
			display_LEDs();
			break;
		case E_MODE_EDIT_KEYBOARD:
			all.leds_base_case |= PIANO_KEYS;
			all.leds_base_case |= (1 << E_SOURCE_BUTTON_KEYBOARD);
			all.leds_base_case |= (1 << E_SOURCE_BUTTON_REC);
			all.leds_base_case |= (1 << E_SOURCE_BUTTON_EDIT);
			display_LEDs();
			break;
	}

}
