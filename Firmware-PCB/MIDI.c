#include "0x2a002a.h"
;
u8  running_status;

/*
** Sets MIDI Note Off.
** channel: 0-15 MIDI Channel Number
** note: 0-127 MIDI Note
** velocity: 0-127 MIDI Velocity
*/
void midi_note_off(u8 channel, u8 note, u8 velocity)
{
    u8  status;
    
    status = E_MS_NOTE_OFF | channel;
    if (velocity == 0 && running_status == E_MS_NOTE_ON | channel)
        ;
    else if (status != running_status)
    {
        UART1_send(status);
        running_status = status;
    }
    UART1_send(note);
    UART1_send(velocity);
}

/*
** Sets MIDI Note On.
** channel: 0-15 MIDI Channel Number
** note: 0-127 MIDI Note
** velocity: 0-127 MIDI Velocity. If 0 => Note Off.
*/
void midi_note_on(u8 channel, u8 note, u8 velocity)
{
    u8 status;

    status = E_MS_NOTE_ON | channel;
    if (status != running_status)
    {
        UART1_send(status);
        running_status = status;
    }
    UART1_send(note);
    UART1_send(velocity);
}

/*
** ???
** channel: 0-15 MIDI Channel Number
** note: 0-127 MIDI Note
** value: 0-127 Pressure Value
*/
void midi_polyphonic_key_pressure(u8 channel, u8 note, u8 value)
{

}

/*
** Changes MIDI Channel Controller Value
** channel: 0-15 MIDI Channel Number
** control: 0-119 MIDI Control
** velocity: 0-127 Control Value
*/
void midi_control_change(u8 channel, u8 control, u8 value)
{
    u8 status;

    status = E_MS_CONTROL_CHANGE | channel;
    if (status != running_status)
    {
        UART1_send(status);
        running_status = status;
    }
    UART1_send(control);
    UART1_send(value);
}

/*
** Changes MIDI Channel Program
** channel: 0-15 MIDI Channel Number
** program: 0-127 MIDI Program Number
*/
void midi_program_change(u8 channel, u8 program)
{

}

/*
** Changes Channel Pressure (Aftertouch)
** channel: 0-15 MIDI Channel Number
** value: 0-127 MIDI Pressure Value
*/
void midi_channel_pressure(u8 channel, u8 value)
{

}

/*
** Changes Pitch Bend
** channel: 0-15 MIDI Channel Number
** value: 0-127 Pitch Bend Change Value
*/
void midi_pitch_bend_change(u8 channel)
{
}


//void    midi_gm_on(void)
//{
//    UART1_send(0xF0);
//    UART1_send(0x7E);
//    UART1_send(0x7F);
//    UART1_send(0x09);
//    UART1_send(0x01);
//    UART1_send(0xF7);
//}
//
//void    midi_gm_off(void)
//{
//    UART1_send(0xF0);
//    UART1_send(0x7E);
//    UART1_send(0x7F);
//    UART1_send(0x09);
//    UART1_send(0x02);
//    UART1_send(0xF7);
//}