#include <xc.h>
#include <sys/attribs.h>
#include "0x2a002a.h"

extern u8          current_mode;
extern u8          cur_instrument;
extern u8          cur_note;
extern u8          cur_octave;
extern u8          cur_active_pattern[QTIME_PER_PATTERN][NOTES_PER_QTIME][ATTRIBUTES_PER_NOTE];
extern u8          active_instrument[PATTERNS_PER_INSTRUMENT][QTIME_PER_PATTERN][NOTES_PER_QTIME][ATTRIBUTES_PER_NOTE];
extern u8          qtime;
extern u16         active_instruments_u16;
extern u8          HT16_write_leds_request;
extern u8          sequencer_notes[MAX_NOTES_PER_QTIME];
extern u8          sequencer_velocities[MAX_NOTES_PER_QTIME];
extern u8          sequencer_notes_count;
extern u8          noteskeys[13];
extern u8          cur_pattern;
extern u8          playing;
u32                current_leds_on;
u32                leds_base_case = 0;

void    display_LEDs(void)
{
    u8 i;
    u32 to_toggle;

    to_toggle = current_leds_on ^ leds_base_case;
    i = 0;
    while (i < 32)
    {
        if (to_toggle & (1 << i))
            led_toggle(i);
        ++i;
    }
    current_leds_on = leds_base_case;
}

void    display_LEDs_for_qtime(void)
{
    u8 i;
    u32 new_display;
    u32 to_toggle;
    static  u8  blink = 0;

    new_display = leds_base_case;
    if (playing == MUSIC_PLAYING || (blink = !blink) == 1)
        new_display ^= (1 << qtime);
    to_toggle = current_leds_on ^ new_display;
    i = 0;
    while (i < 32)
    {
        if (to_toggle & (1 << i))
            led_toggle(i);
        ++i;
    }
    current_leds_on = new_display;
}

void    display_LEDs_static_blink(u8 led)
{
    u8 i;
    u32 new_display;
    u32 to_toggle;
    static  u8  blink = 0;

    new_display = leds_base_case;
    if ((blink = !blink) == 1)
        new_display ^= (1 << led);
    to_toggle = current_leds_on ^ new_display;
    i = 0;
    while (i < 32)
    {
        if (to_toggle & (1 << i))
            led_toggle(i);
        ++i;
    }
    current_leds_on = new_display;
}

void    update_leds_base_case(void)
{
    u8  qt = 0;
    u8  n;
    u8  i;
    u8  p;

    leds_base_case = 0;
    if (playing == MUSIC_PLAYING)
        leds_base_case |= (1 << E_SOURCE_BUTTON_PLAY_PAUSE);
    leds_base_case |= (1 << cur_octave + E_SOURCE_ENCODER_0);
    switch (current_mode)
    {
        case E_MODE_INSTRU:
            while (qt < INSTRUMENTS_COUNT)
            {
                if (active_instruments_u16 & (1 << qt))
                    leds_base_case |= (1 << qt);
                qt++;
            }
            leds_base_case |= (1 << E_SOURCE_BUTTON_INSTRUMENT);
            display_LEDs();
            break;
        case E_MODE_EDIT_INSTRU:
            while (qt < INSTRUMENTS_COUNT)
            {
                if (active_instruments_u16 & (1 << qt))
                    leds_base_case |= (1 << qt);
                qt++;
            }
            leds_base_case |= (1 << E_SOURCE_BUTTON_INSTRUMENT);
            display_LEDs();
            break;
        case E_MODE_PATTERN:
            while (qt < QTIME_PER_PATTERN)
            {
                n = 0;
                while (n < NOTES_PER_QTIME)
                    if (cur_active_pattern[qt][n++][0] == cur_note)
                        leds_base_case |= (1 << qt);
                qt++;
            }
            leds_base_case |= (1 << E_SOURCE_BUTTON_PATTERN);
            display_LEDs_for_qtime();
            break;
        case E_MODE_EDIT_PATTERN:
            p = 0;
            while (p < PATTERNS_PER_INSTRUMENT)
            {
                qt = 0;
                while (qt < QTIME_PER_PATTERN)
                    if (active_instrument[p][qt++][0][0] != NO_NOTE)
                    {
                        leds_base_case |= (1 << p);
                        break ;
                    }
                ++p;
            }
            leds_base_case |= (1 << E_SOURCE_BUTTON_EDIT);
            leds_base_case |= (1 << E_SOURCE_BUTTON_PATTERN);
            display_LEDs_static_blink(cur_pattern);
            break;
        case E_MODE_EDIT_KEYBOARD:
            leds_base_case |= PIANO_KEYS;
            leds_base_case |= (1 << E_SOURCE_BUTTON_KEYBOARD);
            leds_base_case |= (1 << E_SOURCE_BUTTON_EDIT);
            display_LEDs();
            break;
        case E_MODE_KEYBOARD:
            leds_base_case |= PIANO_KEYS;
            leds_base_case |= (1 << E_SOURCE_BUTTON_KEYBOARD);
            if (sequencer_notes_count)
            {
                i = 0;
                while (i < sequencer_notes_count)
                {
                    if (sequencer_notes[i] / 12 == cur_octave)
                        leds_base_case ^= 1 << (noteskeys[sequencer_notes[i] % 12]);
                    else if (sequencer_notes[i] == 12 * (cur_octave + 1))
                        leds_base_case ^= 1 << (E_SOURCE_KEY_15);
                    ++i;
                }
            }
            display_LEDs();
            break;
    }

}