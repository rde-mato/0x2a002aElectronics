#include <xc.h>
#include <sys/attribs.h>
#include "0x2a002a.h"

u8              qtime = 0;
extern u32      leds_status;
extern u8	cur_note;
extern u8       current_mode;
extern u32      leds_active;
extern u8       pattern_mode;
extern u8	active_patterns[INSTRUMENTS_COUNT][QTIME_PER_INSTRUMENT][NOTES_PER_QTIME][ATTRIBUTES_PER_NOTE];

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
        midi_note_off(00, notes[i], velocities[i++]);
    }
    return ;
}

void    display_LEDs_for_qtime(u8 qt)
{
//    static u32 last_display = 0;
    u8 i;
    u32 new_display;
    u32 to_toggle;

    new_display = leds_active ^ (1 << qt);
    to_toggle = leds_status ^ new_display;
    i = 0;
    while (i < 16)
    {
        if (to_toggle & (1 << i))
            led_toggle(i);
        ++i;
    }
    leds_status = new_display;

}

void __ISR(_TIMER_2_VECTOR, IPL3AUTO) Timer2QTime(void)
{
    TIMER2_INT_FLAG_CLR;

    if (current_mode == E_MODE_DEFAULT)
    {
        if (leds_active & (1 << qtime))
            send_MIDI_for_qtime(qtime);
        display_LEDs_for_qtime(qtime);
    }
    qtime = (qtime + 1) & 15;
}