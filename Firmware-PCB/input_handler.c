#include <xc.h>
#include <sys/attribs.h>
#include "0x2a002a.h"

//
//extern u32     current_key_scan;

u32 cpt = 0;
//
extern u8 led;

void event_handler(u8 event_type, u32 event_source)
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
			led_toggle(led);
                        led = (led + 1) & 0xF;
                        led_toggle(led);
			break;
		case E_ENCODER_TURNED_LEFT:
			led_toggle(led);
                        led = (led - 1) & 0xF;
                        led_toggle(led);
			break;


	}
}
