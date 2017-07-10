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

void    generate_notes_list_for_qtime(u8 qt)
{
    u8  instrument;
    u8  note;
    u8  n;

    sequencer_notes_count = 0;
    note = 0;
    while (note < NOTES_PER_QTIME && sequencer_notes_count < MAX_NOTES_PER_QTIME)
    {
        if ((n = cur_active_pattern[qt][note][0]) != NO_NOTE)
        {
            sequencer_notes[sequencer_notes_count] = n;
            sequencer_velocities[sequencer_notes_count] = cur_active_pattern[qt][note][1];
            if (++sequencer_notes_count >= 32)
                break;
            else
                note++;
        }
        else
            note = NOTES_PER_QTIME;
    }

    instrument = 0;
    while (instrument < INSTRUMENTS_COUNT && sequencer_notes_count < MAX_NOTES_PER_QTIME)
    {
        if (instrument != cur_instrument)
        {
            note = 0;
            while (note < NOTES_PER_QTIME && sequencer_notes_count < MAX_NOTES_PER_QTIME)
            {
                if ((n = active_patterns_array[instrument][qt][note][0]) != NO_NOTE)
                {
                    sequencer_notes[sequencer_notes_count] = n;
                    sequencer_velocities[sequencer_notes_count] = active_patterns_array[instrument][qt][note][1];
                    if (++sequencer_notes_count >= 32)
                        break;
                    else
                        note++;
                }
                else
                    note = NOTES_PER_QTIME;
            }
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
        midi_note_on(00, sequencer_notes[i], sequencer_velocities[i]);
        midi_note_off(00, sequencer_notes[i], sequencer_velocities[i]);
        ++i;
    }
    return ;
}


void    timer_2_init(void)
{
    TIMER2_STOP_AND_RESET;
    TIMER2_VALUE = 0;
    TIMER2_PERIOD = (FREQUENCY * 15) / (MIDI_PPQN * 256 /100 * INITIAL_BPM_x100);
    TIMER2_PRESCALE = TIMER_PRESCALE_256;
}

void    int_init_timer2(void)
{
    TIMER2_INT_FLAG_CLR;
    TIMER2_INT_PRIORITY = 7;
    TIMER2_INT_ENABLE = INT_ENABLED;
}

void __ISR(_TIMER_2_VECTOR, IPL7AUTO) Timer2QTime(void)
{
    static u8 ppqn_count = 0;

    TIMER2_INT_FLAG_CLR;

    if (++ppqn_count == MIDI_PPQN)
    {
        if (current_mode == E_MODE_PATTERN)
            display_LEDs_for_qtime();
        if (playing == MUSIC_PLAYING)
        {

            generate_notes_list_for_qtime(qtime);
            send_MIDI_for_qtime();
            if (current_mode == E_MODE_KEYBOARD)
                update_leds_base_case();
            qtime = (qtime + 1) & 15;
        }
        ppqn_count = 0;

    }
//    UART1_send(TIMING_CLOCK);
}