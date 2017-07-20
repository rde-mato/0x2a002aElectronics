#include <xc.h>
#include <sys/attribs.h>
#include "0x2a002a.h"

u8              qtime = 0;
u8              sequencer_notes[MAX_NOTES_PER_QTIME] = { 0 };
u8              sequencer_velocities[MAX_NOTES_PER_QTIME] = { 0 };
u8              sequencer_notes_count = 0;
extern u32      current_leds_on;
extern u8	cur_note;
extern u8       cur_instrument;
extern u8       current_mode;
extern u32      leds_base_case;
extern u8       pattern_mode;
extern u8	active_patterns_array[INSTRUMENTS_COUNT][QTIME_PER_PATTERN][NOTES_PER_QTIME][ATTRIBUTES_PER_NOTE];
extern u8       cur_active_pattern[QTIME_PER_PATTERN][NOTES_PER_QTIME][ATTRIBUTES_PER_NOTE];
extern u8       playing;

void    qtime_generate_note_off(u8 instrument, u8 last_qt[][ATTRIBUTES_PER_NOTE], u8 new_qt[][ATTRIBUTES_PER_NOTE])
{
    u8  i;
    u8  j;
    u8  still;
    u8  old_n;
    u8  new_n;

    for (i = 0; i < NOTES_PER_QTIME; i++)
    {
        old_n = last_qt[i][E_NOTE_VALUE];
        if (old_n == NO_NOTE)
            break;
        still = 0;
        for (j = 0; j < NOTES_PER_QTIME; j++)
        {
            new_n = new_qt[j][E_NOTE_VALUE];
            if (new_n == NO_NOTE)
                break;
            if (NOTE_VALUE(new_n) == NOTE_VALUE(old_n))
            {
                if (IS_NOTE_CONTINUOUS(new_n))
                    still = 1;
                break;
            }
        }
        if (!still)
            midi_note_off(instrument, NOTE_VALUE(old_n), last_qt[i][E_NOTE_VELOCITY]);
    }
}

void qtime_generate_note_on(u8 instrument, u8 qt[][ATTRIBUTES_PER_NOTE])
{
    u8  i;
    u8  *new_n;

    for (i = 0; i < NOTES_PER_QTIME; i++)
    {
        new_n = qt[i];
        if (new_n[E_NOTE_VALUE] == NO_NOTE)
            break;
        else if (IS_NOTE_ATTACK(new_n[E_NOTE_VALUE]))
            midi_note_on(instrument, NOTE_VALUE(new_n[E_NOTE_VALUE]), new_n[E_NOTE_VELOCITY]);
    }
}

void    timer_2_init(void)
{
    TIMER2_STOP_AND_RESET;
    TIMER2_VALUE = 0;
    TIMER2_PERIOD = (FREQUENCY * 15) / (256 /100 * INITIAL_BPM_x100);
    TIMER2_PRESCALE = TIMER_B_PRESCALE_256;
}

void    int_init_timer2(void)
{
    TIMER2_INT_FLAG_CLR;
    TIMER2_INT_PRIORITY = 7;
    TIMER2_INT_ENABLE = INT_ENABLED;
}

void __ISR(_TIMER_2_VECTOR, IPL7AUTO) Timer2QTime(void)
{
    TIMER2_INT_FLAG_CLR;

    u8 i;

    display_LEDs();
    if (playing == MUSIC_PLAYING)
    {
        for (i = 0; i < INSTRUMENTS_COUNT; i++)
        {
            if (i == cur_instrument)
                qtime_generate_note_off(cur_instrument, cur_active_pattern[(qtime - 1) & 0xF], cur_active_pattern[qtime]);
            else
                qtime_generate_note_off(i, active_patterns_array[i][(qtime - 1) & 0xF], active_patterns_array[i][qtime]);
        }
        for (i = 0; i < INSTRUMENTS_COUNT; i++)
        {
            if (i == cur_instrument)
                qtime_generate_note_on(cur_instrument, cur_active_pattern[qtime]);
            else
                qtime_generate_note_on(i, active_patterns_array[i][qtime]);
        }

        if (current_mode == E_MODE_KEYBOARD)
            update_leds_base_case();
        qtime = (qtime + 1) & 15;
    }
}