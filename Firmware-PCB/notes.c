#include <xc.h>
#include <sys/attribs.h>
#include "0x2a002a.h"

//extern float g_bpm;
//
//extern u32  current_leds_on;
//extern u8   qtime;
//extern u8   HT16_write_leds_request;
//extern u8   current_template;
//extern u8   tap_index;
//extern u32  leds_base_case;
//u8          edit_pressed = 0;
//u8          tap_pressed = 0;
//
extern u8          active_patterns[INSTRUMENTS_COUNT][QTIME_PER_PATTERN][NOTES_PER_QTIME][ATTRIBUTES_PER_NOTE];
extern u8          cur_instrument;
extern u8          cur_note;
extern u8          cur_velocity;

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

void    pop_note(u8 qtime)
{
    u8 i = 0;


    while (i < NOTES_PER_QTIME - 1)
    {
        if (active_patterns[cur_instrument][qtime][i][0] == cur_note)
            break ;
        ++i;
    }
    while (i < NOTES_PER_QTIME - 1)
    {
        active_patterns[cur_instrument][qtime][i][0] = active_patterns[cur_instrument][qtime][i + 1][0];
        active_patterns[cur_instrument][qtime][i][1] = active_patterns[cur_instrument][qtime][i + 1][1];
        ++i;
    }
    active_patterns[cur_instrument][qtime][NOTES_PER_QTIME - 1][0] = 0;
    active_patterns[cur_instrument][qtime][NOTES_PER_QTIME - 1][1] = 0;
}


void    add_note(u8 qt)
{
    u8  i = 0;

    while (i < NOTES_PER_QTIME)
    {
        if (active_patterns[cur_instrument][qt][i][E_NOTE_VALUE] == cur_note)
        {
            if (active_patterns[cur_instrument][qt][i][E_NOTE_VELOCITY] == cur_velocity)
                pop_note(qt);
            else
                active_patterns[cur_instrument][qt][i][E_NOTE_VELOCITY] = cur_velocity;
            break;
        }
        i++;
    }
    if (i == NOTES_PER_QTIME)
        push_note(cur_instrument, qt, cur_note, cur_velocity);
    update_leds_base_case();
}