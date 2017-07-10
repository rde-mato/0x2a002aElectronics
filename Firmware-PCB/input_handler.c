#include <xc.h>
#include <sys/attribs.h>
#include "0x2a002a.h"

extern u8   encoder_midi_cc[8];
extern u8   qtime;
extern u8   HT16_write_leds_request;
extern u8   current_template;
extern u8   tap_index;
extern u32  leds_base_case;
extern u32  bpm_x100;
u8          edit_pressed = 0;
u8          tap_pressed = 0;

u8          cur_active_pattern[QTIME_PER_PATTERN][NOTES_PER_QTIME][ATTRIBUTES_PER_NOTE] = { NO_NOTE };
u8          active_patterns_array[INSTRUMENTS_COUNT][QTIME_PER_PATTERN][NOTES_PER_QTIME][ATTRIBUTES_PER_NOTE];
u8          active_instrument[PATTERNS_PER_INSTRUMENT][QTIME_PER_PATTERN][NOTES_PER_QTIME][ATTRIBUTES_PER_NOTE];
u16         active_instruments_u16 = 0b01010111;
u8          active_pattern_per_instrument[INSTRUMENTS_COUNT] = { 0 };
u8          encoders_values[8] = { 0x0F };
u8          encoders_scale[8] = { 16 };

u8          cur_instrument = 0;
u8          cur_pattern = 0;
u8          cur_note = 36;
u8          current_mode = E_MODE_PATTERN;
u8          cur_octave = 3;
u8          cur_velocity = 0x40;
u8          cur_encoder;
u8          playing = MUSIC_PAUSE;

void	keys_handler(u8 event_type, u8 event_source)
{
    s8  n;

    switch (current_mode)
    {
        case E_MODE_PATTERN:
        {
            switch (event_type)
            {
                case E_KEY_PRESSED:
                    add_note(event_source);
                    update_leds_base_case();
                    break;
                case E_KEY_RELEASED:
                    break;
                case E_KEY_LONG_PRESSED:
                    break;
            }
            break ;
        }
        case E_MODE_KEYBOARD:
        {
            switch (event_type)
            {
                    case E_KEY_PRESSED:
                        if ((n = key_to_note(event_source, cur_octave)) == -1)
                                break;
                        cur_note = n;
                        midi_note_on(00, cur_note, cur_velocity);
                        update_leds_base_case();
                        request_template(TEMPLATE_NOTE);
                        break;
                    case E_KEY_RELEASED:
                        midi_note_off(00, cur_note, cur_velocity);
                            break;
                    case E_KEY_LONG_PRESSED:
                            break;
            }
            break ;
        }
        case E_MODE_EDIT_INSTRU:
        {
            switch (event_type)
            {
                    case E_KEY_PRESSED:
                        cur_instrument = event_source;
                        cur_pattern = active_pattern_per_instrument[cur_instrument];
                        load_cur_instrument_from_eeprom();
                        request_template(TEMPLATE_INSTRUMENT);
                        break;
                    case E_KEY_RELEASED:
                        break;
                    case E_KEY_LONG_PRESSED:
                        break;
            }
            break ;
        }
    }
}

void	encoders_handler(u8 event_type, u8 event_source)
{

    cur_encoder = event_source - E_SOURCE_ENCODER_0;
    switch (event_type)
    {
        case E_ENCODER_TURNED_RIGHT:
            if (edit_pressed)
            {
                
            }
            else
            {
                if (encoders_values[cur_encoder] != 0xFF)
                    encoders_values[cur_encoder]++;
                if (encoders_values[cur_encoder] & 0x01)
                {
                    midi_control_change(0x00, encoder_midi_cc[cur_encoder], encoders_values[cur_encoder] / 2);
                    request_template(TEMPLATE_ENCODER);
                }
            }
            break;
        case E_ENCODER_TURNED_LEFT:
            if (edit_pressed)
            {

            }
            else
            {
                if (encoders_values[cur_encoder] != 0x00)
                    encoders_values[cur_encoder]--;
                if (encoders_values[cur_encoder] & 0x01)
                {
                    midi_control_change(0x00, encoder_midi_cc[cur_encoder], encoders_values[cur_encoder] / 2);
                    request_template(TEMPLATE_ENCODER);
                }
            }
            break;
        case E_KEY_PRESSED:
            cur_octave = cur_encoder;
            cur_note = cur_note % 12 + 12 * cur_octave;
            update_leds_base_case();
            request_template(TEMPLATE_OCTAVE);
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
                        request_template(TEMPLATE_MAIN_MENU_START);
                    break;
		case E_KEY_RELEASED:
                    break;
		case E_KEY_LONG_PRESSED:
                    break;
		case E_ENCODER_TURNED_RIGHT:
                    if(tap_pressed)
                    {
                        tap_index = 0;
                        change_bpm(10, 0);
                        request_template(TEMPLATE_BPM);
                    }
                    else if (edit_pressed)
                    {
                        cur_velocity++;
                        request_template(TEMPLATE_VELOCITY);
                    }
                    else
                    {
                    }
                    break;
		case E_ENCODER_TURNED_LEFT:
                    if(tap_pressed)
                    {
                        tap_index = 0;
                        change_bpm(-10, 0);
                        request_template(TEMPLATE_BPM);
                    }
                    else if (edit_pressed)
                    {
                        cur_velocity--;
                        request_template(TEMPLATE_VELOCITY);
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
                    playing = !playing;
                    update_leds_base_case();
                    /*********/

//                    led_toggle(E_SOURCE_BUTTON_PLAY_PAUSE);
//			eeprom_buf_size = 8;
//			eeprom_address = 0;
//			lcd_strncpy(eeprom_buf, "abcd 345", 8);
//			SPI_eeprom_write_request = 1;
                    
//            eeprom_write("abcd 345", 8, 128);
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
//                    led_set(E_SOURCE_BUTTON_CUE);
//                    led_toggle((qtime - 1) & 15);
//                    TIMER2_VALUE = 0;
//                    qtime = 0;
                    /*******/
//			eeprom_buf_size = 8;
//			lcd_bzero(eeprom_buf, 8);
//			eeprom_address = 0;
//            eeprom_read(8, 128);
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
                    if (edit_pressed)
                        current_mode = E_MODE_EDIT_INSTRU;
                    else
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
            if (!tap_pressed)
                bpm_new_tap();
            tap_pressed = 1;
            request_template(TEMPLATE_BPM);
            break;
        case E_KEY_RELEASED:
            tap_pressed = 0;
            bpm_new_tap_release();
            request_template(TEMPLATE_BPM);
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
                    if(edit_pressed)
                        save_cur_pattern_to_eeprom();

//            eeprom_read_buf(haha, 8);
//            LCD_putnstr(0, 0, haha, 8);

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
//                        led_set(E_SOURCE_BUTTON_EDIT);
			break;
		case E_KEY_RELEASED:
			edit_pressed = 0;
//                        led_clr(E_SOURCE_BUTTON_EDIT);
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
