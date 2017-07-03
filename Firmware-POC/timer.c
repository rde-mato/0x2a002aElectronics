#include <xc.h>
#include <sys/attribs.h>
#include "0x2a002a.h"

extern char pattern[16][4][3];
u32     bpm = 50;
u8      qtime = 0;

    
void set_bpm(void)
{
	PR2 = FREQUENCY /(bpm / 15); // bpm * 4 / 60
}

void __ISR(_TIMER_3_VECTOR, IPL3AUTO) Timer3Handler(void)
{
    IFS0bits.T3IF = 0;

//        char    line[21] = "...................";
//
//        line[qtime] = 'a' + qtime;
//        LCD_putstr(qtime & 7, 0, line);
//        LCD_print_changed_chars();

    	LED_ON_OFF = !LED_ON_OFF;
//
//	led_toggle((qtime - 1) & 0x0F);
//	led_toggle(qtime);
//	qtime = (qtime + 1) & 0x0F;
	TMR2 = 0;
}
