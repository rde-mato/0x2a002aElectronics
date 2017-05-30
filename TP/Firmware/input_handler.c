#include <xc.h>
#include <sys/attribs.h>
#include "0x2a002a.h"


extern u32     current_key_scan;

u8  index = 0;

void event_handler(u8 event_type, u8 event_source)
{
    switch (event_type)
    {
        case E_KEY_PRESSED:
            led_set(event_source);
            break;
        case E_KEY_RELEASED:
            led_toggle(event_source);
            break;
        case E_KEY_LONG_PRESSED:
            led_toggle(event_source);
            break;
        case E_ENCODER_TURNED_RIGHT:
            if (event_source == E_SOURCE_ENCODER_MAIN)
            {
                led_toggle(index);
                --index;
                index &= 0x0F;
                led_toggle(index);
            }
            break;
        case E_ENCODER_TURNED_LEFT:
            if (event_source == E_SOURCE_ENCODER_MAIN)
            {
                led_toggle(index);
                ++index;
                index &= 0x0F;
                led_toggle(index);
            }
            break;


    }
}