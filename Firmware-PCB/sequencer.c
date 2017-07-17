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

inline void    qtime_generate_pattern_notes(u8 qt, u8 pattern[][NOTES_PER_QTIME][ATTRIBUTES_PER_NOTE])
{
    u8  note;
    u8  n;

    note = 0;
    while (note < NOTES_PER_QTIME && sequencer_notes_count < MAX_NOTES_PER_QTIME)
    {
        if ((n = pattern[qt][note][E_NOTE_VALUE]) != NO_NOTE)
        {
            if (IS_NOTE_ATTACK(n))
            {
                sequencer_notes[sequencer_notes_count] = n;
                sequencer_velocities[sequencer_notes_count] = pattern[qt][note][1];
                if (++sequencer_notes_count >= 32)
                    return ;
            }
            note++;
        }
        else
            note = NOTES_PER_QTIME;
    }
}

//void    qtime_generate_all_notes(u8 qt)
//{
//    u8  instrument;
//    u8  note;
//    u8  prev_qt;
//    u8  n;
//
//    prev_qt = (qt - 1) & 0xF;
//    for (instrument = 0; instrument < INSTRUMENTS_COUNT; instrument++)
//    {
//        for (note = 0; note < NOTES_PER_QTIME; note++)
//        {
//            if ((n = active_patterns_array[instrument][qt][]))
//            if (active_patterns_array[instrument][prev_qt][note][E_NOTE_VALUE])
//            if (NO_NOTE)
//                break;
//        }
//    }
//}

void    qtime_generate_all_notes(u8 qt)
{
    u8  instrument;

    sequencer_notes_count = 0;

    qtime_generate_pattern_notes(qt, cur_active_pattern);

    instrument = 0;
    while (instrument < INSTRUMENTS_COUNT && sequencer_notes_count < MAX_NOTES_PER_QTIME)
    {
        if (instrument != cur_instrument)
        {
            qtime_generate_pattern_notes(qt, active_patterns_array[instrument]);
            if (sequencer_notes_count >= 32)
                break;
        }
        ++instrument;
    }
}

void    send_MIDI_for_qtime(void)
{
    u8  i;

    i = 0;
    while (i < sequencer_notes_count)
    {
        midi_note_on(00, NOTE_VALUE(sequencer_notes[i]), sequencer_velocities[i]);
        midi_note_off(00, NOTE_VALUE(sequencer_notes[i]), sequencer_velocities[i]);
        ++i;
    }
    return ;
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
    display_LEDs();
    if (playing == MUSIC_PLAYING)
    {
        qtime_generate_all_notes(qtime);
        send_MIDI_for_qtime();
        if (current_mode == E_MODE_KEYBOARD)
            update_leds_base_case();
        qtime = (qtime + 1) & 15;
    }

//    UART1_send(TIMING_CLOCK);
}