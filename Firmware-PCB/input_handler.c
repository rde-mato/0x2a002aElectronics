#include <xc.h>
#include <sys/attribs.h>
#include "0x2a002a.h"

extern struct s_all all;

static const enum E_MIDI_CONTROLLERS	encoder_midi_cc[8] =
{
	MC_GP_CONTROLLER_1,
	MC_GP_CONTROLLER_2,
	MC_GP_CONTROLLER_3,
	MC_GP_CONTROLLER_4,
	MC_GP_CONTROLLER_5,
	MC_GP_CONTROLLER_6,
	MC_GP_CONTROLLER_7,
	MC_GP_CONTROLLER_8,
};

void	update_mode(u8 mode)
{
	all.cur_mode = mode;
}

void	keys_handler(u8 event_type, u8 event_source)
{
	s8			n;
	u8			i;
	u8			quantiz;
	static u8	piano_roll_start[13] = {
							NO_NOTE, NO_NOTE, NO_NOTE, NO_NOTE, NO_NOTE,
							NO_NOTE, NO_NOTE, NO_NOTE, NO_NOTE, NO_NOTE,
							NO_NOTE, NO_NOTE, NO_NOTE
							};

	switch (all.cur_mode)
	{
		case E_MODE_PATTERN:
			{
				switch (event_type)
				{
					case E_KEY_PRESSED:
						add_note(event_source, 0, E_NOTE_ATTACK);
						update_leds_base_case();
						break;
				}
				break ;
			}
		break;

		case E_MODE_EDIT_PATTERN:
			{
				switch (event_type)
				{
					case E_KEY_PRESSED:
						update_cur_pattern(event_source);
						break;
				}
				break ;
			}
		break;

		case E_MODE_KEYBOARD:
			{
				if ((n = key_to_note(event_source, all.cur_octave)) == -1)
					break;
				switch (event_type)
				{
					case E_KEY_PRESSED:
						all.cur_note = n;
						midi_note_on(all.cur_instrument, n, all.cur_velocity);
						update_leds_base_case();
						request_template(TEMPLATE_NOTE);
						break;
					case E_KEY_RELEASED:
						midi_note_off(all.cur_instrument, n, all.cur_velocity);
						break;
				}
				break ;
			}
		case E_MODE_EDIT_KEYBOARD:
			{
				quantiz = TIMER2_VALUE > (TIMER2_PERIOD / 2);
				switch (event_type)
				{
					case E_KEY_PRESSED:
						if ((n = key_to_note(event_source, all.cur_octave)) == -1)
							break;
						all.cur_note = n;
						i = n - 12 * all.cur_octave;
						// if (quantiz)
						midi_note_on(all.cur_instrument, all.cur_note, all.cur_velocity);
						piano_roll_start[i] = ((u8)(all.qtime + quantiz - 1) & 0xF);
						update_leds_base_case();
						request_template(TEMPLATE_NOTE);
						break;
					case E_KEY_RELEASED:
						if ((n = key_to_note(event_source, all.cur_octave)) == -1)
							break;
						all.cur_note = n;
						midi_note_off(all.cur_instrument, all.cur_note, all.cur_velocity);
						i = n - 12 * all.cur_octave;
						n = piano_roll_start[i];
						add_note(n, 1, E_NOTE_ATTACK);
						if (n++ > all.qtime)
						{
							while (n < ((u8)(all.qtime + quantiz - 1) & 0xF))
								add_note(n++, 1, E_NOTE_CONTINUOUS);
						}
						piano_roll_start[i] = -1;
						update_leds_base_case();
						request_template(TEMPLATE_NOTE);
						break;
						break;
				}
				break ;
			}
		case E_MODE_INSTRU:
			{
				switch (event_type)
				{
					case E_KEY_PRESSED:
						all.active_instruments_u16 ^= (1 << event_source);
						if (!(all.active_instruments_u16 & (1 << event_source)))
							midi_control_change(event_source, MCMM_ALL_NOTES_OFF, 0x00);
						update_leds_base_case();
						break;
				}
				break ;
			}
		case E_MODE_EDIT_INSTRU:
			{
				switch (event_type)
				{
					case E_KEY_PRESSED:
						update_cur_instrument(event_source);
						break;
				}
				break ;
			}
	}
}

void	encoders_handler(u8 event_type, u8 event_source)
{
    u16 new_value;

    all.cur_encoder = event_source - E_SOURCE_ENCODER_0;
    switch (event_type)
    {
        case E_ENCODER_TURNED_RIGHT:
            if (EDIT_PRESSED)
            {
                
            }
            else
            {
				if ((new_value = all.encoders_values[all.cur_encoder] + ENCODERS_STEP) <= ENCODERS_MAX)
                {
                    all.encoders_values[all.cur_encoder] = new_value;
                    //encoders_dirty |= 1 << cur_encoder; Entraine des problemes d affichage.
                    midi_control_change(0x00, encoder_midi_cc[all.cur_encoder], new_value);
                    request_template(TEMPLATE_ENCODER);
                }
            }
            break;
        case E_ENCODER_TURNED_LEFT:
            if (EDIT_PRESSED)
            {

            }
            else
            {
				if ((new_value = all.encoders_values[all.cur_encoder] - ENCODERS_STEP) <= ENCODERS_MAX)
                {
                    all.encoders_values[all.cur_encoder] = new_value;
                    //encoders_dirty |= 1 << cur_encoder; Entraine des problemes d affichage.
                    midi_control_change(0x00, encoder_midi_cc[all.cur_encoder], new_value);
                    request_template(TEMPLATE_ENCODER);
                }
            }
            break;
        case E_KEY_PRESSED:
			midi_note_off(all.cur_instrument, all.cur_note, all.cur_velocity);
            all.cur_octave = all.cur_encoder;
            all.cur_note = all.cur_note % 12 + 12 * all.cur_octave;
            update_leds_base_case();
            request_template(TEMPLATE_OCTAVE);
            break;
    }
}

void	main_encoder_handler(u8 event_type)
{
	static u8	prev_turn_direction = E_ENCODER_NO_DIRECTION;

	all.menu_items_count = 0;
	if (SD_IS_PRESENT && !all.SD_initialized)
	{
		if (SD_card_init())
			all.SD_initialized = 1;
	}
	else if (!SD_IS_PRESENT && all.SD_initialized)
		all.SD_initialized = 0;
	if (all.pattern_in_pastebin)
		all.menu_items[all.menu_items_count++] = E_MENU_ITEMS_PASTE_PATTERN;
	all.menu_items[all.menu_items_count++] = E_MENU_ITEMS_COPY_PATTERN;

	if (SD_IS_PRESENT)
	{
		all.menu_items[all.menu_items_count++] = E_MENU_ITEMS_LOAD_FROM_SD;
		all.menu_items[all.menu_items_count++] = E_MENU_ITEMS_LOAD_TO_SD;
	}
	all.menu_items[all.menu_items_count++] = E_MENU_ITEMS_CLEAR_EEPROM;
	all.menu_items[all.menu_items_count++] = E_MENU_ITEMS_EXIT;

	switch (event_type)
	{
		case E_KEY_PRESSED:
			if (all.default_template == TEMPLATE_DEFAULT)
			{
				all.default_template = TEMPLATE_MAIN_MENU;
				all.menu_item_highlighted = 0;
				request_template(TEMPLATE_MAIN_MENU);

			}
			else if (all.default_template == TEMPLATE_MAIN_MENU)
			{
				all.default_template = TEMPLATE_DEFAULT;
				if (all.menu_items[all.menu_item_highlighted] == E_MENU_ITEMS_EXIT)
				{
					request_template(TEMPLATE_DEFAULT);
				}
				else if (all.menu_items[all.menu_item_highlighted] == E_MENU_ITEMS_LOAD_TO_SD)
				{
					sequencer_pause();
					request_template(TEMPLATE_LOADING_IN_PROGRESS);
					copy_EEPROM_to_SD();
				}
				else if (all.menu_items[all.menu_item_highlighted] == E_MENU_ITEMS_LOAD_FROM_SD)
				{
					sequencer_pause();
					request_template(TEMPLATE_LOADING_IN_PROGRESS);
					copy_SD_to_EEPROM();
				}
				else if (all.menu_items[all.menu_item_highlighted] == E_MENU_ITEMS_PASTE_PATTERN)
				{
					memcpy(all.cur_active_pattern, all.pattern_pastebin, QTIME_PER_PATTERN * NOTES_PER_QTIME * ATTRIBUTES_PER_NOTE);
					update_leds_base_case();
					request_template(TEMPLATE_DEFAULT);
				}
				else if (all.menu_items[all.menu_item_highlighted] == E_MENU_ITEMS_COPY_PATTERN)
				{
					all.pattern_in_pastebin = 1;
					memcpy(all.pattern_pastebin, all.cur_active_pattern, QTIME_PER_PATTERN * NOTES_PER_QTIME * ATTRIBUTES_PER_NOTE);
					request_template(TEMPLATE_DEFAULT);
				}
				else if (all.menu_items[all.menu_item_highlighted] == E_MENU_ITEMS_CLEAR_EEPROM)
				{
					eeprom_chip_erase();
					update_leds_base_case();
					request_template(TEMPLATE_CHIP_ERASED);
				}
			}
			break;
		case E_ENCODER_TURNED_RIGHT:
			if(TAP_PRESSED)
			{
				T3CONbits.ON = 0;
				bpm_reset_tap();
				change_bpm(100, 1);
				request_template(TEMPLATE_BPM);
			}
			else if (EDIT_PRESSED)
			{
				if (all.cur_velocity < 127)
					all.cur_velocity++;
				request_template(TEMPLATE_VELOCITY);
			}
			else
			{
				if (prev_turn_direction != E_ENCODER_TURNED_RIGHT)
					prev_turn_direction = E_ENCODER_TURNED_RIGHT;
				else if (all.default_template == TEMPLATE_MAIN_MENU)
				{
					if (all.menu_item_highlighted < all.menu_items_count - 1)
						all.menu_item_highlighted++;
					prev_turn_direction = E_ENCODER_NO_DIRECTION;
					request_template(TEMPLATE_MAIN_MENU);
				}
			}
			break;
		case E_ENCODER_TURNED_LEFT:
			if(TAP_PRESSED)
			{
				T3CONbits.ON = 0;
				bpm_reset_tap();
				change_bpm(-100, 1);
				request_template(TEMPLATE_BPM);
			}
			else if (EDIT_PRESSED)
			{
				if (all.cur_velocity > 0)
					all.cur_velocity--;
				request_template(TEMPLATE_VELOCITY);
			}
			else
			{
				if (prev_turn_direction != E_ENCODER_TURNED_LEFT)
					prev_turn_direction = E_ENCODER_TURNED_LEFT;
				else if (all.default_template == TEMPLATE_MAIN_MENU)
				{
					if (all.menu_item_highlighted > 0)
						all.menu_item_highlighted--;
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
			if (all.playing)
				sequencer_pause();
			else
				sequencer_play();
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
			sequencer_pause();
			all.qtime = 0;
			sequencer_play();
			update_leds_base_case();
			break;
		case E_KEY_RELEASED:
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
			if (EDIT_PRESSED)
				all.cur_mode = E_MODE_EDIT_INSTRU;
			else
				all.cur_mode = E_MODE_INSTRU;
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
			if (EDIT_PRESSED)
				all.cur_mode = E_MODE_EDIT_PATTERN;
			else
				all.cur_mode = E_MODE_PATTERN;
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
			if (EDIT_PRESSED)
				all.cur_mode = E_MODE_EDIT_KEYBOARD;
			else
				all.cur_mode = E_MODE_KEYBOARD;
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
			if (!TAP_PRESSED)
				bpm_new_tap();
			request_template(TEMPLATE_BPM);
			break;
		case E_KEY_RELEASED:
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
			if(EDIT_PRESSED)
				save_cur_pattern_to_eeprom();
			else if (all.cur_mode == E_MODE_KEYBOARD)
			{
				all.cur_mode = E_MODE_EDIT_KEYBOARD;
				update_leds_base_case();
			}
			else if (all.cur_mode == E_MODE_EDIT_KEYBOARD)
			{
				all.cur_mode = E_MODE_KEYBOARD;
				update_leds_base_case();
			}
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
			break;
		case E_KEY_RELEASED:
			break;
		case E_KEY_LONG_PRESSED:
			break;
	}
}

void combination_handler(u32 source)
{
	u8 start;
	u8 finish;
	u8 i;
	u8 all_notes_on;

	switch (all.cur_mode)
	{
		case E_MODE_PATTERN:
			i = 0;
			while (i < 32 && !(source & (1 << i)))
				i++;
			start = i++;
			while (i < 32 && !(source & (1 << i)))
				i++;
			finish = i;
			if (start >= E_SOURCE_KEY_0 && finish <= E_SOURCE_KEY_15)
			{
				if (all.qtime >= start && all.qtime < finish && all.leds_base_case & (1 << all.qtime))
                    midi_note_off(all.cur_instrument, all.cur_note, all.cur_velocity);
				all_notes_on = 1;
				i = start + 1;
				while (all_notes_on && i < finish)
				{
					if (!(all.leds_base_case & (1 << i++)))
						all_notes_on = 0;
				}
				all_notes_on &= !(all.leds_base_case & (1 << start));
				all_notes_on &= !(all.leds_base_case & (1 << finish));
				i = start;
				if (all_notes_on)
				{
					while (i <= finish)
						remove_note(i++);
				}
				else
				{
					add_note(i++, 1, E_NOTE_ATTACK);
					while (i <= finish)
						add_note(i++, 1, E_NOTE_CONTINUOUS);
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
