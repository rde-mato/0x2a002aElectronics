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

        INT_init();

        //HT16_init();

        //LCD_display_on_off(0);

        SS_MCP_LCD = 0x0;
        SPI2BUF = 0x4012;
        while (!SPI2STATbits.SPIRBF) ;
//        res = SPI2BUF;
        SPI2BUF = 0xffff;
        while (!SPI2STATbits.SPIRBF) ;
        SPI2BUF = 0x0404;
        while (!SPI2STATbits.SPIRBF) ;
        SPI2BUF = 0xffff;
        while (!SPI2STATbits.SPIRBF) ;
        SPI2BUF = 0x5050;
        while (!SPI2STATbits.SPIRBF) ;
        SPI2BUF = 0x8888;
        while (!SPI2STATbits.SPIRBF) ;
//        res = SPI2BUF;
        SS_MCP_LCD = 0x1;

        T2CONbits.ON = 1;
        
	while (42)
	{
//		manage_I2C2();
//		manage_SPI2();
		WDTCONbits.WDTCLR = 1; // CLEAR WATCHDOG
	}
	return (0);
}
