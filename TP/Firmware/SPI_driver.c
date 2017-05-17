#include <xc.h>
#include <sys/attribs.h>
#include "0x2a002a.h"
//
////#define SPI2READ 0
////#define SPI2WRITE 1
////u8  SPI2_RW = SPI2WRITE;

#define SPI2_WRITE_BUF_MAX 10
#define SPI2_READ_BUF_MAX 10
//
////u8  SPI2_DONE_TEST = 0;
////
////u8  SPI2_write_request = 0;
////u8  SPI2_read_request = 0;
////
u16  SPI2_write_buf[SPI2_WRITE_BUF_MAX] = { 0 };
u8  SPI2_write_buf_index = 0;
u8  SPI2_write_buf_size = 0;

u16  SPI2_read_buf[SPI2_READ_BUF_MAX] = { 0 };
u8  SPI2_read_buf_index = 0;
u8  SPI2_read_request_count = 0;
//
////u8  SPI2_state = E_SPI2_DONE;
////
////read_callback SPI2_read_cb = NULL;
////write_callback SPI2_write_cb = NULL;
////
////u8 test = 0;
////
////void    SPI2_push(u16 data)
////{
////    SPI2_write_buf[SPI2_write_buf_size++] = data;
////}
////
////void dummy_cb(void)
////{
////    SPI2_DONE_TEST = 1;
////    return ;
////}
////
////void    manage_SPI2(void)
////{
////    u16 test[3] = {0x4009, 0xAA41, 0x0900};
////
////    if (SPI2_write_request)
////    {
////        SPI2_write_request = 0;
////        SPI2_write_callback(test, 3, &dummy_cb);
////    }
////}
////
////void    SPI2_state_machine(void)
////{
////    u16  sent_value;
////
////    switch (SPI2_state)
////    {
////        case E_SPI2_WRITE:
////            if (SPI2STATbits.SPIBUSY)
////                break;
////            if (SPI2_write_buf_index == SPI2_write_buf_size)
////                SPI2_state = E_SPI2_WRITE_CALLBACK;
////            else
////            {
////                LATGbits.LATG9 = 0x0;
////                sent_value = SPI2_write_buf[SPI2_write_buf_index++];
////                SPI2BUF = sent_value;
////                break;
////            }
////        case E_SPI2_WRITE_CALLBACK:
////            LATGbits.LATG9 = 0x1;
////            if (SPI2_write_cb)
////                (*SPI2_write_cb)();
////            SPI2_write_buf_index = 0;
////            SPI2_write_buf_size = 0;
////            SPI2_write_cb = NULL;
////            SPI2_state = E_SPI2_DONE;
////            break;
////
////    }
////}
////
////void __ISR(_SPI_2_VECTOR, IPL4AUTO) SPI2_Handler(void) {
////    IFS1bits.SPI2TXIF = 0;
////    IFS1bits.SPI2RXIF = 0;
////    SPI2_state_machine();       // les deux interrupts sont systematiquement raised au meme moment
////}
////
////
////void SPI2_write_callback(u16 *buffer, u8 size, write_callback cb)
////{
////    u8  i;
////
////    if (!buffer || size <= 0)
////        return ;
////    i = 0;
////    while (i < size)
////        SPI2_push(buffer[i++]);
////    SPI2_state = E_SPI2_WRITE;
////    SPI2_write_cb = cb;
////    IFS1bits.SPI2TXIF = 1; // interrupt pour lancer la state machine
////
////}
//

static void SPI2_send_next_byte(void) {
    if (SPI2_write_buf_index < SPI2_write_buf_size)
    {
        SPI2BUF = SPI2_write_buf[SPI2_write_buf_index++];
    }
    else
    {
        SPI2_write_buf_index = 0;
        SPI2_write_buf_size = 0;
    }
}

void __ISR(_SPI_2_VECTOR, IPL4AUTO) SPI2_Handler(void) {

    //Ici on perd bcp de bande passante (~ moitie selon le pico)
    //parce que l'on attend que le PIC ait bien shift l'input dans le registre.
    //C'est necessaire car c'est le seul moment ou on est bien sur que la transmission
    //est idle pour toggle le chip select ~sigh~
    IFS1bits.SPI2RXIF = 0;
    LATGbits.LATG9 = 0x1;
    LATGbits.LATG9 = 0x0;
    SPI2_read_buf[SPI2_read_buf_index++] = SPI2BUF;
    SPI2_send_next_byte();
}

SPI2_send(void) {
    LATGbits.LATG9 = 0x0;
    SPI2_send_next_byte();
}
