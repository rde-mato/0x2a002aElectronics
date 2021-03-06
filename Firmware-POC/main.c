#include <xc.h>
#include <sys/attribs.h>
#include "0x2a002a.h"

#define INSTRUMENTS_COUNT       16
#define PATTERNS_PER_INSTRUMENT 16
#define QTIME_PER_INSTRUMENT    16
#define NOTES_PER_QTIME         4
#define ATTRIBUTES_PER_NOTE     3

//  1 pattern = 192 u8
//  1 instrument = 3072 u8

u8	active_patterns[INSTRUMENTS_COUNT][QTIME_PER_INSTRUMENT][NOTES_PER_QTIME][ATTRIBUTES_PER_NOTE];
u8	active_instrument[PATTERNS_PER_INSTRUMENT][QTIME_PER_INSTRUMENT][NOTES_PER_QTIME][ATTRIBUTES_PER_NOTE];
u8      pattern_pointers[INSTRUMENTS_COUNT][PATTERNS_PER_INSTRUMENT];

u8	cur_instrument;
u8	cur_pattern;
u8	cur_note;
u8	cur_octave;
u8	cur_velocity;
u8	bpm;

u8	encoders_memory[16][8];
u8	encoders_play[16][8];
//u16	notes_memory[16][16][16][4]; //trop grand pour etre dans la memoire mais a aller taper direct dans la flash
u16	notes_play[16][16][4];
u16	active_instruments;

//	pour chaque instrument actif on ne garde que le pattern actif
//	

u8  led = 0;

int main(void)
{
    u8  ret = 0;
    u32 block = 512;

        GPIO_init();    // bien mettre a jour avec la vraie board
//        TIMER_init();
//	I2C2_init();
	SPI2_init();
        //MCP_LCD_init();
//        LCD_init();
//        MCP_ENCODERS_init();

        SD_card_init();
        ret = SD_card_read_block(block);
        ret = SD_card_write_block(block);
        ret = SD_card_read_block(block);

//        INT_init();
//        HT16_init();


        T2CONbits.ON = 1;
//        led_toggle(led);
	while (42)
	{
//		manage_I2C2();
//		manage_SPI2();
		WDTCONbits.WDTCLR = 1; // CLEAR WATCHDOG
	}
	return (0);
}
