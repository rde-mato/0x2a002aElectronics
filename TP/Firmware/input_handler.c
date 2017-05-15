#include <xc.h>
#include <sys/attribs.h>
#include "0x2a002a.h"


extern u32     current_key_scan;


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
    }
}