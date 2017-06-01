#include <xc.h>
#include <sys/attribs.h>
#include "0x2a002a.h"

char	pattern[16][4][3];

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
u8	active_patterns[16];

//	pour chaque instrument actif on ne garde que le pattern actif
//	


int main(void)
{
	init_pattern();

	GPIO_init();
	TIMER_init();
	I2C2_init();
	SPI2_init();

	INT_init();

	HT16_init();
	//MCP_init();
	//MCP_write(0x04, 0xFFFF); //pour les encodeurs
	T2CONbits.ON = 1;

	//led_toggle(0);

	MCP_init_LCD();

	//aller chercher dans la flash les valeurs pour chaque pattern actif

	LCD_display_on_off(1);

	while (42)
	{
		manage_I2C2();
		manage_SPI2();
		WDTCONbits.WDTCLR = 1; // CLEAR WATCHDOG
	}
	return (0);
}
