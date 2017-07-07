#include <xc.h>
#include <sys/attribs.h>
#include "0x2a002a.h"

extern float g_bpm;

extern u8   encoder_midi_cc[8];
extern u32  current_leds_on;
extern u8   qtime;
extern u8   HT16_write_leds_request;
extern u8   current_template;
extern u8   tap_index;
extern u32  leds_base_case;
u8          edit_pressed = 0;
u8          tap_pressed = 0;

u8          active_patterns[INSTRUMENTS_COUNT][QTIME_PER_PATTERN][NOTES_PER_QTIME][ATTRIBUTES_PER_NOTE] = { 0 };
u8          active_instrument[PATTERNS_PER_INSTRUMENT][QTIME_PER_PATTERN][NOTES_PER_QTIME][ATTRIBUTES_PER_NOTE] = { 0 };
u16         active_instruments_u16 = 0b01010111;
u8          encoders_values[8] = { 0x0F };

u8          cur_instrument = 0;
u8          cur_pattern = 0;
u8          cur_note = 36;
u8          current_mode = E_MODE_PATTERN;
u8          cur_octave = 3;
u8          cur_velocity = 0x40;
u8          cur_encoder;



void	keys_handler(u8 event_type, u8 event_source)
{
    s8  n;

    if (current_mode == E_MODE_PATTERN)
    {
	switch (event_type)
	{
		case E_KEY_PRESSED:
                    add_note(event_source);
			break;
		case E_KEY_RELEASED:
			break;
		case E_KEY_LONG_PRESSED:
			break;
	}
    }
    else if (current_mode == E_MODE_KEYBOARD)
    {
	switch (event_type)
	{
		case E_KEY_PRESSED:
                    if ((n = key_to_note(event_source, cur_octave)) == -1)
                            break;
                    cur_note = n;
                    midi_note_on(00, cur_note, cur_velocity);
                    midi_note_off(00, cur_note, cur_velocity);
                    update_leds_base_case();
                    request_template(&template_note);
                    break;
		case E_KEY_RELEASED:
			break;
		case E_KEY_LONG_PRESSED:
			break;
	}

    }
}

void	encoders_handler(u8 event_type, u8 event_source)
{
    cur_encoder = event_source - E_SOURCE_ENCODER_0;
    switch (event_type)
    {
        case E_ENCODER_TURNED_RIGHT:
            if (encoders_values[cur_encoder] != 0xFF)
                encoders_values[cur_encoder]++;
            if (encoders_values[cur_encoder] & 0x01)
            {
                midi_control_change(0x00, encoder_midi_cc[cur_encoder], encoders_values[cur_encoder] / 2);
                request_template(&template_encoder);
            }
            break;
        case E_ENCODER_TURNED_LEFT:
            if (encoders_values[cur_encoder] != 0x00)
                encoders_values[cur_encoder]--;
            if (encoders_values[cur_encoder] & 0x01)
            {
                midi_control_change(0x00, encoder_midi_cc[cur_encoder], encoders_values[cur_encoder] / 2);
                request_template(&template_encoder);
            }
            break;
    }
}
void	main_encoder_handler(u8 event_type)
{
    static u8 truc = 0;
	if (current_mode != E_MODE_MENU)
	{
		switch (event_type)
		{
		case E_KEY_PRESSED:
			current_mode == E_MODE_MENU;
                        midi_control_change(00, truc++, 40);
                        request_template(&template_main_menu_start);
                    break;
		case E_KEY_RELEASED:
                    break;
		case E_KEY_LONG_PRESSED:
                    break;
		case E_ENCODER_TURNED_RIGHT:
                    if(tap_pressed)
                    {
                        tap_index = 0;
                        change_bpm(+1.);
                        request_template(&template_bpm);
                    }
                    else if (edit_pressed)
                    {
                        cur_velocity++;
                        request_template(&template_velocity);
                    }
                    else
                    {
                    }
                    break;
		case E_ENCODER_TURNED_LEFT:
                    if(tap_pressed)
                    {
                        tap_index = 0;
                        change_bpm(-1.);
                        request_template(&template_bpm);
                    }
                    else if (edit_pressed)
                    {
                        cur_velocity--;
                        request_template(&template_velocity);
                    }
                    else
                    {
                    }
                    break;
		}
	}
	else
	{
	}
}

void	button_play_handler(u8 event_type)
{
	switch (event_type)
	{
		case E_KEY_PRESSED:
                    MUSIC_PLAYING = !MUSIC_PLAYING;
                    update_leds_base_case();

//                    led_toggle(E_SOURCE_BUTTON_PLAY_PAUSE);
//			eeprom_buf_size = 8;
//			eeprom_address = 0;
//			lcd_strncpy(eeprom_buf, "abcd 345", 8);
//			SPI_eeprom_write_request = 1;
			break;
		case E_KEY_RELEASED:
			break;
		case E_KEY_LONG_PRESSED:
			break;
	}
}

void	button_cue_handler(u8 event_type)
{
	switch (event_type)
	{
		case E_KEY_PRESSED:
                    led_set(E_SOURCE_BUTTON_CUE);
                    led_toggle((qtime - 1) & 15);
                    TIMER2_VALUE = 0;
                    qtime = 0;
//			eeprom_buf_size = 8;
//			lcd_bzero(eeprom_buf, 8);
//			eeprom_address = 0;
//			SPI_eeprom_read_request = 1;
			break;
		case E_KEY_RELEASED:
//                    led_toggle(qtime - 1);
                    led_clr(E_SOURCE_BUTTON_CUE);
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
                    current_mode = E_MODE_INSTRU;
                    update_leds_base_case();
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
                    current_mode = E_MODE_PATTERN;
                    update_leds_base_case();
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
                    if (edit_pressed)
                        current_mode = E_MODE_EDIT_KEYBOARD;
                    else
                        current_mode = E_MODE_KEYBOARD;
                    update_leds_base_case();
                    break;
		case E_KEY_RELEASED:
//                    display_keyboard();
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
            tap_pressed = 1;
            bpm_new_tap();
            request_template(&template_bpm);
            break;
        case E_KEY_RELEASED:
            tap_pressed = 0;
            bpm_new_tap_release();
            request_template(&template_bpm);
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
//			LCD_putnstr(0, 0, eeprom_buf, 8);
//			LCD_print_changed_chars();
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
                        led_set(E_SOURCE_BUTTON_EDIT);
			break;
		case E_KEY_RELEASED:
			edit_pressed = 0;
                        led_clr(E_SOURCE_BUTTON_EDIT);
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
	else if (event_source == E_SOURCE_BUTTON_CUE)
		button_cue_handler(event_type);
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
