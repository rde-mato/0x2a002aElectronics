#include "0x2a002a.h"

extern u8          current_mode;
extern u8          cur_instrument;
extern u8          cur_note;
extern u8          cur_pattern;
extern u8          cur_octave;
extern u8          cur_active_pattern[QTIME_PER_PATTERN][NOTES_PER_QTIME][ATTRIBUTES_PER_NOTE];
extern u8          active_instrument[PATTERNS_PER_INSTRUMENT][QTIME_PER_PATTERN][NOTES_PER_QTIME][ATTRIBUTES_PER_NOTE];
extern u8          qtime;
extern u16         active_instruments_u16;
extern u8          HT16_write_leds_request;
extern u8          noteskeys[13];
extern u8          playing;
extern u8          cue_pressed;
u32                current_leds_on;
u32                leds_base_case = 0;

void    display_LEDs(void)
{
    u8          i;
    u8          blinking = 0;
    u32         to_display;
    u32         to_toggle;
    u8          to_blink;
    static u8   blink = 0;

    to_display = leds_base_case;
    switch (current_mode)
    {
        case E_MODE_INSTRU:
            blinking = 1;
            to_blink = cur_instrument;
            break;
        case E_MODE_EDIT_INSTRU:
            blinking = 1;
            to_blink = cur_instrument;
            break;
        case E_MODE_PATTERN:
            if (!(playing == MUSIC_PLAYING))
            {
                blinking = 1;
                to_blink = qtime;
            }
            else
                to_display ^= (1 << qtime);
            break;
        case E_MODE_EDIT_PATTERN:
            blinking = 1;
            to_blink = cur_pattern;
            break;
        case E_MODE_KEYBOARD:
            break;
        case E_MODE_EDIT_KEYBOARD:
            break;
    }
    blink = !blink;
    if (blinking && blink == 1)
        to_display ^= (1 << to_blink);
    to_toggle = current_leds_on ^ to_display;
    i = 0;
    while (i < 32)
    {
        if (to_toggle & (1 << i))
            led_toggle(i);
        ++i;
    }
    current_leds_on = to_display;
}

u32     keyboard_get_playing_notes(void)
{
    u8  i;
    u8  j;
    u8  note;
    u32 ret;

    ret = 0;
    for (i = 0; i < NOTES_PER_QTIME; i++)
    {
        note = cur_active_pattern[qtime][i][E_NOTE_VALUE];
        if (note == NO_NOTE)
            break;
        note = NOTE_VALUE(note);
        if (note / 12 == cur_octave)
            ret ^= 1 << (noteskeys[note % 12]);
        else if (note == 12 * (cur_octave + 1))
            ret ^= 1 << (E_SOURCE_KEY_15);
    }
    return (ret);
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
    if (cue_pressed)
        leds_base_case |= (1 << E_SOURCE_BUTTON_CUE);
    leds_base_case |= (1 << (cur_octave + E_SOURCE_ENCODER_0));
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
            leds_base_case |= (1 << E_SOURCE_BUTTON_EDIT);
            display_LEDs();
            break;
        case E_MODE_PATTERN:
            while (qt < QTIME_PER_PATTERN)
            {
                n = 0;
                while (n < NOTES_PER_QTIME)
                    if (NOTE_VALUE(cur_active_pattern[qt][n++][0]) == cur_note)
                        leds_base_case |= (1 << qt);
                qt++;
            }
            leds_base_case |= (1 << E_SOURCE_BUTTON_PATTERN);
            display_LEDs();
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
            display_LEDs();
            break;
        case E_MODE_KEYBOARD:
            leds_base_case |= PIANO_KEYS;
            leds_base_case |= (1 << E_SOURCE_BUTTON_KEYBOARD);
            leds_base_case ^= keyboard_get_playing_notes();
            display_LEDs();
            break;
        case E_MODE_EDIT_KEYBOARD:
            leds_base_case |= PIANO_KEYS;
            leds_base_case |= (1 << E_SOURCE_BUTTON_KEYBOARD);
            leds_base_case |= (1 << E_SOURCE_BUTTON_REC);
            leds_base_case |= (1 << E_SOURCE_BUTTON_EDIT);
            display_LEDs();
            break;
    }

}
