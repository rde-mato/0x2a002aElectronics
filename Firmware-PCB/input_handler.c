#include <xc.h>
#include <sys/attribs.h>
#include "0x2a002a.h"

//extern u8	led;		// a degager

extern float g_bpm;
extern u32      leds_status;
extern u8   qtime;
u32         leds_active = 0;

extern u8      HT16_write_leds_request;
u8          edit_pressed = 0;
extern u8   current_mode;
u8          tap_pressed = 0;

u8	active_patterns[INSTRUMENTS_COUNT][QTIME_PER_INSTRUMENT][NOTES_PER_QTIME][ATTRIBUTES_PER_NOTE] = { 0 };
u8	active_instrument[PATTERNS_PER_INSTRUMENT][QTIME_PER_INSTRUMENT][NOTES_PER_QTIME][ATTRIBUTES_PER_NOTE] = { 0 };
u16     active_instruments_u16;
u8      encoders_values[8] = { 0x0F };

u8	cur_instrument = 0;
u8	cur_pattern = 0;
u8	cur_note = 36;
u8	cur_octave = 3;
u8	cur_velocity = 0x40;

void    push_note(u8 instrument, u8 qtime, u8 note, u8 velocity)
{
    u8 i = NOTES_PER_QTIME - 1;

    while (i)
    {
        active_patterns[instrument][qtime][i][0] = active_patterns[instrument][qtime][i - 1][0];
        active_patterns[instrument][qtime][i][1] = active_patterns[instrument][qtime][i - 1][1];
        --i;
    }
    active_patterns[instrument][qtime][0][0] = note;
    active_patterns[instrument][qtime][0][1] = velocity;
}

void    pop_note(u8 instrument, u8 qtime)
{
    u8 i = 0;

    while (i < NOTES_PER_QTIME - 1)
    {
        active_patterns[instrument][qtime][i][0] = active_patterns[instrument][qtime][i + 1][0];
        active_patterns[instrument][qtime][i][1] = active_patterns[instrument][qtime][i + 1][1];
        ++i;
    }
    active_patterns[instrument][qtime][NOTES_PER_QTIME - 1][0] = 0;
    active_patterns[instrument][qtime][NOTES_PER_QTIME - 1][1] = 0;
}

void    update_leds(void)
{
    u8  qt = 0;

    leds_active = 0;
    if (current_mode == E_MODE_DEFAULT)
    {
        while (qt < QTIME_PER_INSTRUMENT)
        {
            if (active_patterns[cur_instrument][qt][0][0])
                leds_active |= (1 << qt);
            else
                leds_active &= ~(1 << qt);
            qt++;
        }
    }
}

void    add_note(u8 qt)
{
    u8  i = 0;

    while (i < NOTES_PER_QTIME)
    {
        if (active_patterns[cur_instrument][qt][i][0] == cur_note)
        {
            if (active_patterns[cur_instrument][qt][i][1] == cur_velocity)
                pop_note(cur_instrument, qt);
            else
                active_patterns[cur_instrument][qt][i][1] = cur_velocity;
            break;
        }
        i++;
    }
    if (i == NOTES_PER_QTIME)
        push_note(cur_instrument, qt, cur_note, cur_velocity);
    update_leds();
}

void	keys_handler(u8 event_type, u8 event_source)
{
    if (current_mode == E_MODE_DEFAULT)
    {
	switch (event_type)
	{
		case E_KEY_PRESSED:
                    add_note(event_source);
//                    led_toggle(event_source);

			break;
		case E_KEY_RELEASED:
//			led_toggle(event_source);
			break;
		case E_KEY_LONG_PRESSED:
//			led_toggle(event_source);
			break;
	}
    }
}

void	encoders_handler(u8 event_type, u8 event_source)
{
    u8 encoder_number;

    encoder_number = event_source - E_SOURCE_ENCODER_0;
    switch (event_type)
    {
        case E_ENCODER_TURNED_RIGHT:
            template_encoder(encoder_number, encoders_values[encoder_number]++);
//            if (event_source == E_SOURCE_ENCODER_0)
//            {
//                template_encoder(encoder_number, encoders_values[encoder_number]++);
//                break;
//            }
//            else
//            {
//                midi_send_control_change(0x00, 0x10, 0xFF);
//            }
            break;
        case E_ENCODER_TURNED_LEFT:
            template_encoder(encoder_number, encoders_values[encoder_number]--);
//            if (event_source == E_SOURCE_ENCODER_0)
//            {
//                cpt--;
//                template_encoder(event_source, cpt);
//            }
//            else
//            {
//                midi_send_control_change(0x00, 0x10, 0x00);
//            }
            break;
    }
}
void	main_encoder_handler(u8 event_type)
{
	if (current_mode != E_MODE_MENU)
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
                    if(tap_pressed)
                    {
                        set_bpm((u16)g_bpm + 1);              //TODO: routine
                        template_bpm();
                    }
                    else if (edit_pressed)
                    {
                        cur_velocity++;
                        template_velocity();
                    }
                    else
                    {
                        cur_note++;
                        template_note();
                    }
                    break;
		case E_ENCODER_TURNED_LEFT:
                    if(tap_pressed)
                    {
                        set_bpm((u16)g_bpm - 1);              //TODO: routine
                        template_bpm();
                    }
                    else if (edit_pressed)
                    {
                        cur_velocity--;
                        template_velocity();
                    }
                    else
                    {
                        cur_note--;
                        template_note();
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
                    T2CONbits.ON = !T2CONbits.ON;
                    led_toggle(E_SOURCE_BUTTON_PLAY_PAUSE);
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
//			eeprom_buf_size = 8;
//			lcd_bzero(eeprom_buf, 8);
//			eeprom_address = 0;
//			SPI_eeprom_read_request = 1;
			break;
		case E_KEY_RELEASED:
//                    led_toggle(qtime - 1);
                    led_clr(E_SOURCE_BUTTON_CUE);
                    led_toggle((qtime - 1) & 15);
                    TIMER2_VALUE = 0;
                    qtime = 0;
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
                    display_instrument();
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
                    display_pattern();
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
                        template_bpm();
			break;
		case E_KEY_RELEASED:
                    tap_pressed = 0;
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
			LCD_putnstr(0, 0, eeprom_buf, 8);
			LCD_print_changed_chars();
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
