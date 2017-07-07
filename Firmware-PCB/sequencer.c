#include <xc.h>
#include <sys/attribs.h>
#include "0x2a002a.h"

u8              qtime = 0;
extern u32      current_leds_on;
extern u8	cur_note;
extern u8       current_mode;
extern u32      leds_base_case;
extern u8       pattern_mode;
extern u8	active_patterns[INSTRUMENTS_COUNT][QTIME_PER_PATTERN][NOTES_PER_QTIME][ATTRIBUTES_PER_NOTE];
extern u8       playing;

void    send_MIDI_for_qtime(u8 qt)
{
    u8  notes[32] = { 0 };
    u8  velocities[32] = { 0 };
    u8  notes_count = 0;
    u8  instrument;
    u8  note;
    u8  n;
    u8  i;

    instrument = 0;
    while (instrument < INSTRUMENTS_COUNT && notes_count < 32)
    {
        note = 0;
        while (note < NOTES_PER_QTIME && notes_count < 32)
        {
            if ((n = active_patterns[instrument][qt][note][0]) != 0)
            {
                notes[notes_count] = n;
                velocities[notes_count] = active_patterns[instrument][qt][note][1];
                if (++notes_count >= 32)
                    break;
                else
                    note++;
            }
            else
                note = NOTES_PER_QTIME;
        }
        if (notes_count >= 32)
            break;
        ++instrument;
    }

    i = 0;
    while (i < notes_count)
    {
        midi_note_on(00, notes[i], velocities[i]);
        midi_note_off(00, notes[i], velocities[i]);
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
            send_MIDI_for_qtime(qtime);
            qtime = (qtime + 1) & 15;
        }
        ppqn_count = 0;
            UART1_send(TIMING_CLOCK);

    }
//    UART1_send(TIMING_CLOCK);
}