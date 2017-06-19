#include <xc.h>
#include <sys/attribs.h>
#include "0x2a002a.h"

extern char pattern[16][4][3];
u32     bpm = 250;
u8      qtime = 0;

u32 reponse;

    


void send_qtime(void)
{
	u8 note = 0;

	while (pattern[qtime][note][notevalue])
	{
		UART2_send(pattern[qtime][note][notestatus]);
		UART2_send(pattern[qtime][note][notevalue]);
		UART2_send(pattern[qtime][note][notevelo]);
		note = (note + 1) % 4;
	}
}

void set_bpm(void)
{
	PR2 = FREQUENCY /(bpm / 15); // bpm * 4 / 60
}

void __ISR(_TIMER_3_VECTOR, IPL3AUTO) Timer3Handler(void)
{
    static u8  buffer[3] = {0x40, 0x12, 0b001};
    IFS0bits.T3IF = 0;

//    char    line[21] = ".....................";
//
//
//	//  reponse = SPI2_transmit32bits(0x4112FFFF);
//
//        clear_LCD_chars();
//        line[qtime] = 'a' + qtime;
//      	LCD_load_line(qtime % 8, line);
//	print_LCD_chars();

//        u8  last = buffer[2] & 1;
//        buffer[2] = (0b11 & (buffer[2] >> 1)) | (last << 2);
//        SPI2_push_buffer(0, buffer, 3);

    	LED_ON_OFF = !LED_ON_OFF;

//	led_toggle((qtime - 1) & 0x0F);
//	led_toggle(qtime);
	qtime = (qtime + 1) & 0x0F;
	TMR2 = 0;
}
