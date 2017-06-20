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
        GPIO_init();    // bien mettre a jour avec la vraie board
        TIMER_init();
	//I2C2_init();
	SPI2_init();
        MCP_LCD_init();
        LCD_init();
        INT_init();
        //HT16_init();

//        LCD_print_u8(0, 0, 0x00);
//        LCD_print_u8(0, 1, 0x00);
//        LCD_print_u8(0, 2, 0x42);
//        LCD_print_u8(0, 3, 0x7f);
//        LCD_print_u8(0, 4, 0x40);
//        LCD_print_u8(0, 5, 0x00);

        LCD_putstr(0, 0, "salut");
        LCD_putstr(1, 0, "bonjour");
        LCD_print_changed_chars();

        T2CONbits.ON = 1;

	while (42)
	{
//		manage_I2C2();
		manage_SPI2();
		WDTCONbits.WDTCLR = 1; // CLEAR WATCHDOG
	}
	return (0);
}
