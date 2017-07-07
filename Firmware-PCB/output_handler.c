
#include <xc.h>
#include <sys/attribs.h>
#include "0x2a002a.h"

extern u8          current_mode;
extern u8          cur_instrument;
extern u8          cur_note;
extern u8          active_patterns[INSTRUMENTS_COUNT][QTIME_PER_PATTERN][NOTES_PER_QTIME][ATTRIBUTES_PER_NOTE];
extern u8          qtime;
extern u16         active_instruments_u16;
extern u8          HT16_write_leds_request;
u32                current_leds_on;
u32                leds_base_case = 0;
extern u8          playing;

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
        new_display |= leds_base_case ^ (1 << qtime);
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

    leds_base_case = 0;
    if (MUSIC_PLAYING)
        leds_base_case |= (1 << E_SOURCE_BUTTON_PLAY_PAUSE);
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
        case E_MODE_PATTERN:
            while (qt < QTIME_PER_PATTERN)
            {
                n = 0;
                while (n < NOTES_PER_QTIME)
                    if (active_patterns[cur_instrument][qt][n++][0] == cur_note)
                        leds_base_case |= (1 << qt);
                qt++;
            }
            leds_base_case |= (1 << E_SOURCE_BUTTON_PATTERN);
            display_LEDs_for_qtime();
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
            display_LEDs();
            break;
    }

}
