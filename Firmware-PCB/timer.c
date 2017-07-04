#include <xc.h>
#include <sys/attribs.h>
#include "0x2a002a.h"

u8      qtime = 0;
extern u32 leds_status;
extern u8   cpt;

void __ISR(_TIMER_2_VECTOR, IPL3AUTO) Timer2Handler(void)
{
    TIMER2_INT_FLAG_CLR;
//
//        char    line[21] = "...................";
//
//
//        line[qtime] = 'a' + qtime;
//        LCD_putstr(qtime & 7, 0, line);
//        LCD_print_changed_chars();

	led_toggle((qtime - 1) & 15);
        if (leds_status & (1 << qtime))
        {
            UART1_send(0x80);
            UART1_send(cpt);
            UART1_send(0x0F);
            UART1_send(0x90);
            UART1_send(cpt);
            UART1_send(0x0F);
        }
        led_toggle(qtime);
	qtime = (qtime + 1) & 15;
//	TIMER2_VALUE = 0;
}