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
	u8 i = 0;
	u8 y = -1;

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
	LCD_clear();
	LCD_display_start_origin(0);
	LCD_display_set_y_address(1,0);
	LCD_display_set_x_page(1,0);

        ftputstr("Hello World!");

//
//	LCD_display_set_y_address(0);
//	LCD_display_set_x_page(1);



	while (42)
	{
		manage_I2C2();


		WDTCONbits.WDTCLR = 1; // CLEAR WATCHDOG
	}
	return (0);


//y = -1;
//while (++y < 8)
//{
//	LCD_display_set_x_page(y);
//	i = -1;
//	while (++i < 64)
//		LCD_display_write_data(0xaa);
//}
//y = -1;
//while (++y < 8)
//{
//	LCD_display_set_x_page(y);
//	i = -1;
//	while (++i < 64)
//		LCD_display_write_data(~0xaa);
//}
}
