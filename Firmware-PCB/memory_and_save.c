#include <xc.h>
#include <sys/attribs.h>
#include "0x2a002a.h"


extern struct s_all all;

static const size_t	pattern_size = QTIME_PER_PATTERN * NOTES_PER_QTIME * ATTRIBUTES_PER_NOTE;
static const size_t	instrument_size = PATTERNS_PER_INSTRUMENT * QTIME_PER_PATTERN * NOTES_PER_QTIME * ATTRIBUTES_PER_NOTE;
static const size_t	active_patterns_size = INSTRUMENTS_COUNT * QTIME_PER_PATTERN * NOTES_PER_QTIME * ATTRIBUTES_PER_NOTE;




void	initial_eeprom_download(void)
{
	u8	i = 0;
	u8	p = 0;
	u8	qt;
	u8	n;
	u8	read;
	u16	address;

	CS_EEPROM = CS_LINE_DOWN;
	SPI1BUF = E_EEPROM_READ;
	while (SPI1STATbits.SPIBUSY);
	read = SPI1BUF;

	address = 0x0000;
	SPI1BUF = (u8)(address >> 8);
	while (SPI1STATbits.SPIBUSY);
	read = SPI1BUF;
	SPI1BUF = (u8)(address);
	while (SPI1STATbits.SPIBUSY);
	read = SPI1BUF;

	while (p < PATTERNS_PER_INSTRUMENT)
	{
		qt = 0;
		while (qt < QTIME_PER_PATTERN)
		{
			n = 0;
			while (n < NOTES_PER_QTIME)
			{
				SPI1BUF = 0xFF;
				while (SPI1STATbits.SPIBUSY);
				all.active_instrument[p][qt][n][0] = SPI1BUF;
				SPI1BUF = 0xFF;
				while (SPI1STATbits.SPIBUSY);
				all.active_instrument[p][qt][n][1] = SPI1BUF;
				++n;
			}
			++qt;
		}
		++p;
	}
	CS_EEPROM = CS_LINE_UP;

	i = 0;
	while (i < INSTRUMENTS_COUNT)
	{
		CS_EEPROM = CS_LINE_DOWN;
		SPI1BUF = E_EEPROM_READ;
		while (SPI1STATbits.SPIBUSY);
		read = SPI1BUF;

		address = i * instrument_size;
		SPI1BUF = (u8)(address >> 8);
		while (SPI1STATbits.SPIBUSY);
		read = SPI1BUF;
		SPI1BUF = (u8)(address);
		while (SPI1STATbits.SPIBUSY);
		read = SPI1BUF;

		qt = 0;
		while (qt < QTIME_PER_PATTERN)
		{
			n = 0;
			while (n < NOTES_PER_QTIME)
			{
				SPI1BUF = 0xFF;
				while (SPI1STATbits.SPIBUSY);
				all.active_patterns_array[i][qt][n][0] = SPI1BUF;
				SPI1BUF = 0xFF;
				while (SPI1STATbits.SPIBUSY);
				all.active_patterns_array[i][qt][n][1] = SPI1BUF;
				++n;
			}
			++qt;
		}
		++i;
		CS_EEPROM = CS_LINE_UP;
	}
	memcpy(all.cur_active_pattern, all.active_patterns_array[all.cur_instrument][all.cur_pattern], pattern_size);
	update_leds_base_case();
}

void	memory_init(void)
{
	memset(all.cur_active_pattern, 0xFF, pattern_size);
	memset(all.pattern_pastebin, 0xFF, pattern_size);
	memset(all.active_patterns_array, 0xFF, active_patterns_size);
	memset(all.active_instrument, 0xFF, instrument_size);
}

void	update_cur_pattern(u8 new_cur_pattern)
{
	all.cur_pattern = new_cur_pattern;
	all.active_pattern_per_instrument[all.cur_instrument] = new_cur_pattern;
	qtime_generate_note_off(all.cur_instrument, all.cur_active_pattern[all.qtime - 1], all.active_instrument[all.cur_pattern][all.qtime]); // a dégager de la !
	memcpy(all.cur_active_pattern, all.active_instrument[all.cur_pattern], pattern_size);
	request_template(TEMPLATE_PATTERN);
	update_mode(E_MODE_PATTERN);
	update_leds_base_case();

}

void	update_cur_note(u8 note)
{
	all.cur_note = note;
	midi_note_on(all.cur_instrument, note, all.cur_velocity);// a degager
}

void	cb_save_cur_pattern_to_eeprom(void)
{
	request_template(TEMPLATE_PATTERN_RECORDED);
}


void	save_cur_pattern_to_eeprom(void)
{
	memcpy(all.active_instrument[all.cur_pattern], all.cur_active_pattern, pattern_size);
	memcpy(all.active_patterns_array[all.cur_instrument], all.cur_active_pattern, pattern_size);
	eeprom_write_callback((u8 *)all.cur_active_pattern,
			(PATTERNS_PER_INSTRUMENT * all.cur_instrument + all.cur_pattern) * pattern_size,
			pattern_size,
			&cb_save_cur_pattern_to_eeprom);
}

void	cb_cur_instrument_from_eeprom(void)
{
	request_template(TEMPLATE_INSTRUMENT);
	update_mode(E_MODE_PATTERN);
	update_leds_base_case();
}

void	load_cur_instrument_from_eeprom(void)
{
	eeprom_read_callback(all.cur_instrument * instrument_size,
			(u8 *)all.active_instrument,
			instrument_size,
			&cb_cur_instrument_from_eeprom);
}

void	active_instrument_init(void)
{
	load_cur_instrument_from_eeprom();
}

void	update_cur_instrument(u8 new_cur_instrument)
{
	memcpy(all.active_patterns_array[all.cur_instrument], all.cur_active_pattern, pattern_size);
	all.cur_instrument = new_cur_instrument;
	all.cur_pattern = all.active_pattern_per_instrument[new_cur_instrument];
	memcpy(all.cur_active_pattern, all.active_patterns_array[new_cur_instrument], pattern_size);
	load_cur_instrument_from_eeprom();
}