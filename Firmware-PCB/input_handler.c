#include <xc.h>
#include <sys/attribs.h>
#include "0x2a002a.h"

extern u8	led;		// a degager
u8			edit_pressed = 0;
u8			current_mode = E_MODE_DEFAULT;

u8 cpt = 0;

void	keys_handler(u8 event_type, u8 event_source)
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

void	encoders_handler(u8 event_type, u8 event_source)
{
    u8 nbstr[5] = { 0 };

	switch (event_type)
	{
		case E_ENCODER_TURNED_RIGHT:
                        cpt++;
                        lcditoa(cpt, nbstr, 4);
                        LCD_putstr(7, 0, nbstr);
                        LCD_print_changed_chars();
//			led_toggle(led);
//			led = (led + 1) & 0xF;
//			led_toggle(led);
			break;
		case E_ENCODER_TURNED_LEFT:
                        cpt--;
                        lcditoa(cpt, nbstr, 4);
                        LCD_putstr(7, 0, nbstr);
                        LCD_print_changed_chars();
//			led_toggle(led);
//			led = (led - 1) & 0xF;
//			led_toggle(led);
			break;
	}
}
void	main_encoder_handler(u8 event_type)
{
	if (current_mode == E_MODE_DEFAULT)
	{
		switch (event_type)
		{
		case E_KEY_PRESSED:
			current_mode == E_MODE_MENU;
                        template_main_menu(1);
			break;
		case E_KEY_RELEASED:
			break;
		case E_KEY_LONG_PRESSED:
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
	else if (current_mode == E_MODE_MENU)
	{
	}
}

void	button_play_handler(u8 event_type)
{
	switch (event_type)
	{
		case E_KEY_PRESSED:
			break;
		case E_KEY_RELEASED:
			break;
		case E_KEY_LONG_PRESSED:
			break;
	}
}

void	button_stop_handler(u8 event_type)
{
	switch (event_type)
	{
		case E_KEY_PRESSED:
			break;
		case E_KEY_RELEASED:
			break;
		case E_KEY_LONG_PRESSED:
			break;
	}
}

void	button_instrument_handler(u8 event_type)
{
	switch (event_type)
	{
		case E_KEY_PRESSED:
			break;
		case E_KEY_RELEASED:
			break;
		case E_KEY_LONG_PRESSED:
			break;
	}
}

void	button_pattern_handler(u8 event_type)
{
	switch (event_type)
	{
		case E_KEY_PRESSED:
			break;
		case E_KEY_RELEASED:
			break;
		case E_KEY_LONG_PRESSED:
			break;
	}
}

void	button_keyboard_handler(u8 event_type)
{
	switch (event_type)
	{
		case E_KEY_PRESSED:
                    T3CONbits.ON = 1;
			break;
		case E_KEY_RELEASED:
			break;
		case E_KEY_LONG_PRESSED:
			break;
	}
}

void	button_tap_handler(u8 event_type)
{
	switch (event_type)
	{
		case E_KEY_PRESSED:
			break;
		case E_KEY_RELEASED:
			break;
		case E_KEY_LONG_PRESSED:
			break;
	}
}

void	button_rec_handler(u8 event_type)
{
	switch (event_type)
	{
		case E_KEY_PRESSED:
			break;
		case E_KEY_RELEASED:
			break;
		case E_KEY_LONG_PRESSED:
			break;
	}
}

void	button_edit_handler(u8 event_type)
{
	switch (event_type)
	{
		case E_KEY_PRESSED:
			edit_pressed = 1;
			break;
		case E_KEY_RELEASED:
			edit_pressed = 0;
			break;
		case E_KEY_LONG_PRESSED:
			break;
	}
}


void	event_handler(u8 event_type, u32 event_source)
{
	if (event_source >= E_SOURCE_KEY_0 && event_source <= E_SOURCE_KEY_15)
		keys_handler(event_type, event_source);
	else if (event_source >= E_SOURCE_ENCODER_0 && event_source <= E_SOURCE_ENCODER_7)
		encoders_handler(event_type, event_source);
	else if (event_source == E_SOURCE_ENCODER_MAIN)
		main_encoder_handler(event_type);
	else if (event_source == E_SOURCE_BUTTON_PLAY_PAUSE)
		button_play_handler(event_type);
	else if (event_source == E_SOURCE_BUTTON_STOP)
		button_stop_handler(event_type);
	else if (event_source == E_SOURCE_BUTTON_INSTRUMENT)
		button_instrument_handler(event_type);
	else if (event_source == E_SOURCE_BUTTON_PATTERN)
		button_pattern_handler(event_type);
	else if (event_source == E_SOURCE_BUTTON_KEYBOARD)
		button_keyboard_handler(event_type);
	else if (event_source == E_SOURCE_BUTTON_TAP)
		button_tap_handler(event_type);
	else if (event_source == E_SOURCE_BUTTON_REC)
		button_rec_handler(event_type);
	else if (event_source == E_SOURCE_BUTTON_EDIT)
		button_edit_handler(event_type);
}
