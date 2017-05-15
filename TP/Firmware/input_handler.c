#include <xc.h>
#include "0x2a002a.h"
#include <sys/attribs.h>


extern u32     current_key_scan;
extern u8      event_type;
extern u8      event_source;

void __ISR(_CORE_SOFTWARE_0_VECTOR, IPL2AUTO) ButtonActionsHandler(void) {
    IFS0bits.CS0IF = 0;
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