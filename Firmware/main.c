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
    u8  read;

        GPIO_init();    // bien mettre a jour avec la vraie board
        TIMER_init();
	//I2C2_init();
	SPI2_init();
        //MCP_LCD_init();
//        LCD_init();
       // INT_init();
        //HT16_init();
        //MCP_ENCODERS_init();

        SS_FLASH = 0;
        SPI2BUF = 0x50;
        while (!SPI2STATbits.SPIRBF) ;
        read = SPI2BUF;
        SS_FLASH = 1;

        SS_FLASH = 0;
        SPI2BUF = 0x01;
        while (!SPI2STATbits.SPIRBF) ;
        read = SPI2BUF;
        SPI2BUF = 0b00000000;
        while (!SPI2STATbits.SPIRBF) ;
        read = SPI2BUF;
        SS_FLASH = 1;

        
        SS_FLASH = 0;
        SPI2BUF = 0x06;
        while (!SPI2STATbits.SPIRBF) ;
        read = SPI2BUF;
        SS_FLASH = 1;

        read = 10000;
        while (read)
        {
            read --;
        }


        SS_FLASH = 0;
        SPI2BUF = 0x05;
        while (!SPI2STATbits.SPIRBF) ;
        read = SPI2BUF;
        SPI2BUF = 0x00;
        while (!SPI2STATbits.SPIRBF) ;
        read = SPI2BUF;
        SS_FLASH = 1;


        read = 10000;
        while (read)
        {
            read --;
        }
        
        SS_FLASH = 0;
        SPI2BUF = 0x02;
        while (!SPI2STATbits.SPIRBF) ;
        read = SPI2BUF;
        SPI2BUF = 0x00;
        while (!SPI2STATbits.SPIRBF) ;
        read = SPI2BUF;
        SPI2BUF = 0x00;
        while (!SPI2STATbits.SPIRBF) ;
        read = SPI2BUF;
        SPI2BUF = 0x02;
        while (!SPI2STATbits.SPIRBF) ;
        read = SPI2BUF;
        SPI2BUF = 42;
        while (!SPI2STATbits.SPIRBF) ;
        read = SPI2BUF;
        SS_FLASH = 1;

        read = 10000;
        while (read)
        {
            read --;
        }

        SS_FLASH = 1;
        SS_FLASH = 0;
        SPI2BUF = 0x03;
        while (!SPI2STATbits.SPIRBF) ;
        read = SPI2BUF;
        SPI2BUF = 0x00;
        while (!SPI2STATbits.SPIRBF) ;
        read = SPI2BUF;
        SPI2BUF = 0x00;
        while (!SPI2STATbits.SPIRBF) ;
        read = SPI2BUF;
        SPI2BUF = 0x02;
        while (!SPI2STATbits.SPIRBF) ;
        read = SPI2BUF;
        SPI2BUF = 0x00;
        while (!SPI2STATbits.SPIRBF) ;
        read = SPI2BUF;
        SS_FLASH = 1;

//        LCD_putstr(0, 0, "salut");
//        LCD_putstr(1, 0, "bonjour");
//        LCD_print_changed_chars();

        T2CONbits.ON = 1;

	while (42)
	{
//		manage_I2C2();
		manage_SPI2();
		WDTCONbits.WDTCLR = 1; // CLEAR WATCHDOG
	}
	return (0);
}
