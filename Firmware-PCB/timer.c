#include <xc.h>
#include <sys/attribs.h>
#include "0x2a002a.h"

u32     bpm = 142;
u8      qtime = 0;

    
void set_bpm(void)
{
	PR2 = FREQUENCY / (bpm / 15); // bpm * 4 / 60
}

void __ISR(_TIMER_3_VECTOR, IPL3AUTO) Timer3Handler(void)
{
    TIMER3_INT_FLAG_CLR;

        char    line[21] = "...................";


        line[qtime] = 'a' + qtime;
        LCD_putstr(qtime & 7, 0, line);
        LCD_print_changed_chars();

//	led_toggle((qtime - 1) & 15);
//	led_toggle(qtime);
//	qtime = (qtime + 1) & 15;
	TIMER3_VALUE = 0;
}
