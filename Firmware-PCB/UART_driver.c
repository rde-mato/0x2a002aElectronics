#include <xc.h>
#include <sys/attribs.h>
#include "0x2a002a.h"

u8  UART1_buf[0x100];
u8  UART1_buf_index = 0;
u8  UART1_buf_len = 0;

void __ISR(_UART_1_VECTOR, IPL5AUTO) UART1_TX_handler(void) {
    UART1_TX_INT_FLAG_CLR;
    U1TXREG = UART1_buf[UART1_buf_index++];
    if (--UART1_buf_len == 0)
        UART1_TX_INT_ENABLE = INT_DISABLED;
}

void UART1_init(void)
{
    U1MODE = 0;
    U1MODEbits.ON = 0;
    U1MODEbits.BRGH = 0;
    U1BRG = SET_MIDI_BRG;
    U1MODEbits.RTSMD = 1;
    U1MODEbits.UEN = 0b00;
    U1STAbits.UTXEN = 1;
    U1STAbits.URXEN = 0;
    U1MODEbits.ON = 1;
}

void UART1_send(char byte)
{
    UART1_TX_INT_ENABLE = INT_DISABLED;
    UART1_buf[UART1_buf_index + UART1_buf_len++] = byte;
    UART1_TX_INT_ENABLE = INT_ENABLED;
}
