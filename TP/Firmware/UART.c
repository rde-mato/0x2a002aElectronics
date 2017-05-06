#include <xc.h>
#include "tp.h"

void UART2_init(void)
{
    U2MODE = 0;
    U2MODEbits.ON = 0;
    U2MODEbits.BRGH = 0;
    U2BRG = SET_MIDI_BRG;
    U2MODEbits.RTSMD = 1;
    U2MODEbits.UEN = 0b00;
    U2STAbits.UTXEN = 1;
    U2STAbits.URXEN = 0;
    U2MODEbits.ON = 1;
}

void UART2_send(char byte)
{
    while (U2STAbits.UTXBF);
    U2TXREG = byte;
}
