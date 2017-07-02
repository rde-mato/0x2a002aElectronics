#include <xc.h>
#include "0x2a002a.h"

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
    while (U1STAbits.UTXBF);
    U1TXREG = byte;
}
