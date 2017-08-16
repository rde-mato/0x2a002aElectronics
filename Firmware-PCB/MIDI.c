#include "0x2a002a.h"

extern	 midi_callback	midi_send;

void	midi_note_off(u8 channel, u8 note, u8 velocity)
{
	u8	buf[3];
	u8	len = 0;

	buf[len++] = E_MS_NOTE_OFF | channel;
	buf[len++] = note;
	buf[len++] = velocity;
	UART1_push(buf, len);
}

void	midi_note_on(u8 channel, u8 note, u8 velocity)
{
	u8	buf[3];
	u8	len = 0;

	buf[len++] = E_MS_NOTE_ON | channel;
	buf[len++] = note;
	buf[len++] = velocity;
	UART1_push(buf, len);
}

void	midi_control_change(u8 channel, u8 control, u8 value)
{
	u8	buf[3];
	u8	len = 0;

	buf[len++] = E_MS_CONTROL_CHANGE | channel;
	buf[len++] = control;
	buf[len++] = value;
	UART1_push(buf, len);
}
