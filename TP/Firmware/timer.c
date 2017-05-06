#include <xc.h>
#include "tp.h"
#include <sys/attribs.h>

extern char pattern[16][4][3];
char qtime = 0;
u8 twice_hz = 16;

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


void __ISR(_TIMER_3_VECTOR, IPL3AUTO) Timer3Handler(void)
{
    LED_ON_OFF = !LED_ON_OFF;


  //  led_toggle(qtime);
    qtime = (qtime + 1) & 0x0F;
    TMR2 = 0;
    IFS0bits.T3IF = 0;
}
