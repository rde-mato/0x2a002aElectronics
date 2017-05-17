#include <xc.h>
#include <sys/attribs.h>
#include "0x2a002a.h"

extern char pattern[16][4][3];
u32     bpm = 142;
u8      qtime = 0;
extern u8   HT16_read_keys_request;

extern u8  SPI2_write_request;



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
    PR2 = FREQUENCY / 400;//(bpm / 15); // bpm * 4 / 60
}

//extern SPI2_read_buf_index;
//u16 reponse = 0x2323;

void __ISR(_TIMER_3_VECTOR, IPL3AUTO) Timer3Handler(void)
{
    LED_ON_OFF = !LED_ON_OFF;


//        u8 GPIO_BANK_0[4] = {0x41, 0x12, 0x00, 0x00};
//        SPI2_write_callback(GPIO_BANK_0, 4, NULL);

//        u8 GPPU[4] = {0x41, 0x0C, 0x00, 0x00};
//        SPI2_write_callback(GPPU, 4, NULL);
//SPI2_read_buf_index = 0;
//    MCP_init();

    
//
//        SPI2BUF = 0x4112;
//        while (SPI2STATbits.SPIBUSY) ;
//        reponse = SPI2BUF;
//
//
//
//
//
//






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
