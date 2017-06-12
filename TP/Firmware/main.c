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
    u8  res;
    u8  buffer[3] = {0x40, 0x12, 0b001};
//        u8 buf[2] = {0xff, 0xff};

//	init_pattern();

	GPIO_init();    // bien mettre a jour avec la vraie board
        TIMER_init();
	SPI2_init();



//	SPI2CONbits.MODE32 = 1;
//        SS_MCP = 0x0; //SS a 0
//        SPI2BUF = 0x40000000;
//        while (!SPI2STATbits.SPIRBF) ;
//        res = SPI2BUF;
//        SS_MCP = 0x1; //SS a 1
//        SPI2CONbits.MODE32 = 0;


        MCP_LCD_init();

        INT_init();

        SPI2_push_buffer(0, buffer, 3);

        T2CONbits.ON = 1;

//        SS_MCP = 0x0; //SS a 0
//        SPI2BUF = 0x40;
//        while (!SPI2STATbits.SPIRBF) ;
//        res = SPI2BUF;
//        SPI2BUF = 0x12;
//        while (!SPI2STATbits.SPIRBF) ;
//        res = SPI2BUF;
//        SPI2BUF = 0b00000111;
//        while (!SPI2STATbits.SPIRBF) ;
//        res = SPI2BUF;
//        SS_MCP = 0x1; //SS a 1
        
	while (42)
	{
//		manage_I2C2();
		manage_SPI2();
		WDTCONbits.WDTCLR = 1; // CLEAR WATCHDOG
	}
	return (0);
}
