#include <xc.h>
#include "tp.h"
#include <sys/attribs.h>

extern char pattern[16][4][3];
u32     bpm = 142;
u8      qtime = 0;
extern u8   HT16_read_request;
extern u32  poll_count;




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
    PR2 = FREQUENCY / (bpm / 15); // bpm * 4 / 60
}

void __ISR(_TIMER_3_VECTOR, IPL3AUTO) Timer3Handler(void)
{
    LED_ON_OFF = !LED_ON_OFF;

//    led_toggle((qtime - 1) & 0x0F);
//    led_toggle(qtime);
    qtime = (qtime + 1) & 0x0F;
    TMR2 = 0;
    IFS0bits.T3IF = 0;
}


//void __ISR(_TIMER_5_VECTOR, IPL4AUTO) Timer5Handler(void)
//{
//    IFS0bits.T5IF = 0;
//    TMR4 = 0; // a priori pas necessaire
//    ++poll_count;
//    HT16_read_request = 1;
//}
