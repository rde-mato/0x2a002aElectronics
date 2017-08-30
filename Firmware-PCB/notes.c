#include <xc.h>
#include <sys/attribs.h>
#include "0x2a002a.h"

extern struct s_all all;
static const u8	keysnotes[16] = { NO_NOTE, 1, 3, NO_NOTE, 6, 8, 10, NO_NOTE, 0, 2, 4, 5, 7, 9, 11, 12 };


s8	key_to_note(u8 key, u8 octave)
{
	key &= 0b1111;
	if (keysnotes[key] == NO_NOTE)
		return (-1);
	return (12 * octave + keysnotes[key]);
}

u8	note_to_key(u8 key, u8 octave)
{
	key &= 0b1111;
	if (keysnotes[key] == NO_NOTE)
		return (-1);
	return (12 * octave + keysnotes[key]);
}

void	push_note(u8 qtime, u8 note, u8 velocity)
{
	u8	i = NOTES_PER_QTIME - 1;

	while (i)
	{
		all.cur_active_pattern[qtime][i][E_NOTE_VALUE] = all.cur_active_pattern[qtime][i - 1][E_NOTE_VALUE];
		all.cur_active_pattern[qtime][i][E_NOTE_VELOCITY] = all.cur_active_pattern[qtime][i - 1][E_NOTE_VELOCITY];
		--i;
	}
	all.cur_active_pattern[qtime][0][E_NOTE_VALUE] = note;
	all.cur_active_pattern[qtime][0][E_NOTE_VELOCITY] = velocity;
}

void	pop_note(u8 qtime)
{
	u8 i = 0;
	u8 j;


	while (i < NOTES_PER_QTIME - 1)
	{
		if (NOTE_VALUE(all.cur_active_pattern[qtime][i][E_NOTE_VALUE]) == all.cur_note)
		{
			for (j = 0; j < NOTES_PER_QTIME; j++)
			{
				if (NOTE_VALUE(all.cur_active_pattern[(qtime + 1) & 0xF][j][E_NOTE_VALUE]) == all.cur_note)
				{
					all.cur_active_pattern[(qtime + 1) & 0xF][j][E_NOTE_VALUE] &= ~E_NOTE_CONTINUOUS;
					break;
				}
			}
			break ;
		}
		++i;
	}
	while (i < NOTES_PER_QTIME - 1)
	{
		all.cur_active_pattern[qtime][i][E_NOTE_VALUE] = all.cur_active_pattern[qtime][i + 1][E_NOTE_VALUE];
		all.cur_active_pattern[qtime][i][E_NOTE_VELOCITY] = all.cur_active_pattern[qtime][i + 1][E_NOTE_VELOCITY];
		++i;
	}
	all.cur_active_pattern[qtime][NOTES_PER_QTIME - 1][E_NOTE_VALUE] = NO_NOTE;
	all.cur_active_pattern[qtime][NOTES_PER_QTIME - 1][E_NOTE_VELOCITY] = 0;
}


void	add_note(u8 qt, u8 force, u8 note_mod)
{
	u8	i = 0;

	while (i < NOTES_PER_QTIME)
	{
		if (NOTE_VALUE(all.cur_active_pattern[qt][i][E_NOTE_VALUE]) == all.cur_note)
		{
			if (all.cur_active_pattern[qt][i][E_NOTE_VELOCITY] == all.cur_velocity && !force)
				pop_note(qt);
			else
			{
				all.cur_active_pattern[qt][i][E_NOTE_VALUE] |= note_mod;
				all.cur_active_pattern[qt][i][E_NOTE_VELOCITY] = all.cur_velocity;
			}
			break;
		}
		i++;
	}
	if (i == NOTES_PER_QTIME)
		push_note(qt, all.cur_note | note_mod, all.cur_velocity);
}

void	remove_note(u8 qt)
{
	u8 i = 0;

	while (i < NOTES_PER_QTIME)
	{
		if (NOTE_VALUE(all.cur_active_pattern[qt][i][E_NOTE_VALUE]) == all.cur_note)
		{
			pop_note(qt);
			break;
		}
		i++;
	}
}
