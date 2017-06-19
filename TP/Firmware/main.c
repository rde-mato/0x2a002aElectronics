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
    u16 send[30] = { 0 };


	GPIO_init();    // bien mettre a jour avec la vraie board
        TIMER_init();
	//I2C2_init();
	SPI2_init();

        MCP_LCD_init();
        LCD_clear();

        INT_init();

        //HT16_init();

//        send[0] = LCD_instruction_to_enable_low(1, 0, 0, 0, 0b01000000); //    LCD_display_set_y_address(0);
//        send[1] = LCD_instruction_to_enable_low(1, 0, 0, 0, 0b10111000); //    LCD_display_set_x_page(0);
//        send[2] = LCD_instruction_to_enable_low(1, 0, 1, 0, 0b01010101); //     data
//        send[3] = LCD_instruction_to_enable_low(1, 0, 1, 0, 0b00110011); //     data
//        send[4] = LCD_instruction_to_enable_low(1, 0, 1, 0, 0b11011010); //     data
//        SPI2_push_LCD_buffer(send, 5);


        LCD_print_u8(0, 0, 0xFF);
        LCD_print_u8(2, 3, 0xFF);
        LCD_print_u8(2, 4, 0xFF);
        LCD_print_u8(3, 65, 0xFF);



        T2CONbits.ON = 1;
        
	while (42)
	{
//		manage_I2C2();
		manage_SPI2();
		WDTCONbits.WDTCLR = 1; // CLEAR WATCHDOG
	}
	return (0);
}
