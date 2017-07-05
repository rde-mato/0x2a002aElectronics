#include <xc.h>
#include <sys/attribs.h>
#include "0x2a002a.h"

u8  current_mode = E_MODE_DEFAULT;

void set_mode_default(void)
{

    //il faut charger les current instru en coours de play

   current_mode = E_MODE_DEFAULT;
    led_clr(E_SOURCE_BUTTON_INSTRUMENT);
    led_clr(E_SOURCE_BUTTON_PATTERN);
    led_clr(E_SOURCE_BUTTON_KEYBOARD);
    template_default();
}

void display_instrument(void)
{

    //il faut charger les current instru en coours de play

   current_mode = E_MODE_INSTRU;
    led_set(E_SOURCE_BUTTON_INSTRUMENT);
    led_clr(E_SOURCE_BUTTON_PATTERN);
    led_clr(E_SOURCE_BUTTON_KEYBOARD);
}

void display_pattern(void)
{

    //charger le display du pattern en cours

    current_mode = E_MODE_PATTERN;
    led_set(E_SOURCE_BUTTON_PATTERN);
    led_clr(E_SOURCE_BUTTON_INSTRUMENT);
    led_clr(E_SOURCE_BUTTON_KEYBOARD);
}

void display_keyboard(void)
{
    u8 i = 0;


    current_mode = E_MODE_KEYBOARD;
    led_set(E_SOURCE_BUTTON_KEYBOARD);
    led_clr(E_SOURCE_BUTTON_INSTRUMENT);
    led_clr(E_SOURCE_BUTTON_PATTERN);
    while (++i < 16)
        led_set(i);
    led_clr(0);
    led_clr(3);
    led_clr(7);
}