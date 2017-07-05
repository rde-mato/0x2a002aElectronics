#include <xc.h>
#include <sys/attribs.h>
#include "0x2a002a.h"

u8 intru_mode = 0;
u8 pattern_mode = 1;
u8 keyboard_mode = 0;

void display_instrument(void)
{

    //il faut charger les current instru en coours de play


    intru_mode = 1;
    pattern_mode = 0;
    keyboard_mode = 0;
    led_set(E_SOURCE_BUTTON_INSTRUMENT);
    led_clr(E_SOURCE_BUTTON_PATTERN);
    led_clr(E_SOURCE_BUTTON_KEYBOARD);
}

void display_pattern(void)
{

    //charger le display du pattern en cours

    pattern_mode = 1;
    keyboard_mode = 0;
    intru_mode = 0;
    led_set(E_SOURCE_BUTTON_PATTERN);
    led_clr(E_SOURCE_BUTTON_INSTRUMENT);
    led_clr(E_SOURCE_BUTTON_KEYBOARD);
}

void display_keyboard(void)
{
    u8 i = 0;

    keyboard_mode = 1;
    intru_mode = 0;
    pattern_mode = 0;
    led_set(E_SOURCE_BUTTON_KEYBOARD);
    led_clr(E_SOURCE_BUTTON_INSTRUMENT);
    led_clr(E_SOURCE_BUTTON_PATTERN);
    while (++i < 16)
        led_set(i);
    led_clr(0);
    led_clr(3);
    led_clr(7);
}