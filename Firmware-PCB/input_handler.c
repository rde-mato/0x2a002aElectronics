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
extern u32  current_leds_on;
extern u32  current_key_scan;
u8          edit_pressed = 0;
u8          tap_pressed = 0;
u8          cue_pressed = 0;

u8          cur_active_pattern[QTIME_PER_PATTERN][NOTES_PER_QTIME][ATTRIBUTES_PER_NOTE] = { NO_NOTE };
u8          active_patterns_array[INSTRUMENTS_COUNT][QTIME_PER_PATTERN][NOTES_PER_QTIME][ATTRIBUTES_PER_NOTE];
u8          active_instrument[PATTERNS_PER_INSTRUMENT][QTIME_PER_PATTERN][NOTES_PER_QTIME][ATTRIBUTES_PER_NOTE];
u16         active_instruments_u16 = 1;
u8          active_pattern_per_instrument[INSTRUMENTS_COUNT] = { 0 };
u8          encoders_values[8] = { 0x0F };
u8          encoders_scale[8] = { 16 };

u8          cur_instrument = 0;
u8          cur_pattern = 0;
u8          cur_note = 36;
u8          current_mode = E_MODE_PATTERN;
u8          default_template = TEMPLATE_DEFAULT;
u8          cur_octave = 3;
u8          cur_velocity = 0x40;
u8          cur_encoder;
u8          playing = MUSIC_PAUSE;
u8          menu_current_level = 0;
u8          menu_level_1 = 0;
u8          menu_item_highlighted;

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
                    add_note(event_source, 0);
                    update_leds_base_case();
                    break;
                case E_KEY_RELEASED:
                    break;
                case E_KEY_LONG_PRESSED:
                    break;
            }
            break ;
        }
        case E_MODE_EDIT_PATTERN:
        {
            switch (event_type)
            {
                case E_KEY_PRESSED:
                    cur_pattern = event_source;
                    update_after_pattern_change();
                    request_template(TEMPLATE_PATTERN);
                    current_mode = E_MODE_PATTERN;
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
        case E_MODE_INSTRU:
        {
            switch (event_type)
            {
                    case E_KEY_PRESSED:
                        active_instruments_u16 ^= (1 << event_source);
                        update_leds_base_case();
                        break;
                    case E_KEY_RELEASED:
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
//    static u8 truc = 0;
    static u8   prev_turn_direction = E_ENCODER_NO_DIRECTION;

    switch (event_type)
        {
        case E_KEY_PRESSED:
            if (default_template == TEMPLATE_DEFAULT)
            {
                default_template = TEMPLATE_MAIN_MENU;
                menu_item_highlighted = 0;
                request_template(TEMPLATE_MAIN_MENU);

            }
            else if (default_template == TEMPLATE_MAIN_MENU)
            {
                if (menu_item_highlighted == E_MENU_ITEMS_EXIT)
                {
                    default_template = TEMPLATE_DEFAULT;
                    request_template(TEMPLATE_DEFAULT);
                }
                else if (menu_item_highlighted == E_MENU_ITEMS_LOAD_TO_SD)
                {
                    playing = MUSIC_PAUSE;
                    request_template(TEMPLATE_LOADING_STARTED);
                    copy_EEPROM_to_SD();
                    default_template = TEMPLATE_DEFAULT;
                    request_template(TEMPLATE_LOADING_SUCCESSFUL);
                }
                else if (menu_item_highlighted == E_MENU_ITEMS_LOAD_FROM_SD)
                {
                    playing = MUSIC_PAUSE;
                    request_template(TEMPLATE_LOADING_STARTED);
                    copy_SD_to_EEPROM();
                    default_template = TEMPLATE_DEFAULT;
                    request_template(TEMPLATE_LOADING_SUCCESSFUL);
                }
            }
            break;
        case E_ENCODER_TURNED_RIGHT:
            if(tap_pressed)
            {
                T3CONbits.ON = 0;
                tap_index = 0;
                change_bpm(100, 1);
                request_template(TEMPLATE_BPM);
            }
            else if (edit_pressed)
            {
                cur_velocity++;
                request_template(TEMPLATE_VELOCITY);
            }
            else
            {
                if (prev_turn_direction != E_ENCODER_TURNED_RIGHT)
                    prev_turn_direction = E_ENCODER_TURNED_RIGHT;
                else if (default_template == TEMPLATE_MAIN_MENU)
                {
                    if (menu_item_highlighted == E_MENU_ITEMS_EXIT)
                        menu_item_highlighted = 0;
                    else
                        menu_item_highlighted++;
                    prev_turn_direction = E_ENCODER_NO_DIRECTION;
                    request_template(TEMPLATE_MAIN_MENU);
                }
            }
            break;
        case E_ENCODER_TURNED_LEFT:
            if(tap_pressed)
            {
                T3CONbits.ON = 0;
                tap_index = 0;
                change_bpm(-100, 1);
                request_template(TEMPLATE_BPM);
            }
            else if (edit_pressed)
            {
                cur_velocity--;
                request_template(TEMPLATE_VELOCITY);
            }
            else
            {
                if (prev_turn_direction != E_ENCODER_TURNED_LEFT)
                    prev_turn_direction = E_ENCODER_TURNED_LEFT;
                else if (default_template == TEMPLATE_MAIN_MENU)
                {
                    if (menu_item_highlighted > 0)
                        menu_item_highlighted--;
                    else
                        menu_item_highlighted = E_MENU_ITEMS_EXIT;
                    prev_turn_direction = E_ENCODER_NO_DIRECTION;
                    request_template(TEMPLATE_MAIN_MENU);
                }
            }
            break;
        }
}

void	button_play_handler(u8 event_type)
{
    switch (event_type)
    {
        case E_KEY_PRESSED:
            playing = !playing;
            update_leds_base_case();
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
            TIMER2_VALUE = 0;
            qtime = 0;
            cue_pressed = 1;
            update_leds_base_case();
            break;
        case E_KEY_RELEASED:
            cue_pressed = 0;
            update_leds_base_case();
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
                    if (edit_pressed)
                        current_mode = E_MODE_EDIT_PATTERN;
                    else
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

void    combination_handler(u32 source)
{
    u8  start;
    u8  finish;
    u8  i;
    u8  all_notes_on;

    switch (current_mode)
    {
        case E_MODE_PATTERN:
            i = 0;
            while (i < 32 && !(source & (1 << i)))
                i++;
            start = i++;
            while (i < 32 && !(source & (1 << i)))
                i++;
            finish = i;

//            if (i == source)
//            {
//                while (++i < 32 && !(current_key_scan & (1 << i)))
//                    ;
//                start = source;
//                finish = i - 1;
//            }
//            else
//            {
//                start = i + 1;
//                finish = source;
//            }
            if (start >= E_SOURCE_KEY_0 && finish <= E_SOURCE_KEY_15)
            {
                all_notes_on = 1;
                i = start + 1;
                while (all_notes_on && i < finish)
                {
                    if (!(leds_base_case & (1 << i++)))
                        all_notes_on = 0;
                }
                all_notes_on &= !(leds_base_case & (1 << start));
                all_notes_on &= !(leds_base_case & (1 << finish));
                i = start;
                if (all_notes_on)
                {
                    while (i <= finish)
                        remove_note(i++);
                }
                else
                {
                    while (i <= finish)
                        add_note(i++, 1);
                }
                update_leds_base_case();
            }

            break ;
     }
}


void	event_handler(u8 event_type, u32 event_source)
{
    if (event_type == E_KEY_COMBINATION_PRESSED)
        combination_handler(event_source);
    else if (event_source >= E_SOURCE_KEY_0 && event_source <= E_SOURCE_KEY_15)
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
