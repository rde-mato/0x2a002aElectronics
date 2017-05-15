#include <xc.h>
#include <sys/attribs.h>
#include "0x2a002a.h"


//void __ISR(_EXTERNAL_3_VECTOR, IPL2AUTO) Main_encoder_Handler(void) {
//    IFS0CLR = 0x8000; // Clear INT3IF
//    event_handler(PORTDbits.RD11 ? E_ENCODER_TURNED_RIGHT : E_ENCODER_TURNED_LEFT, E_SOURCE_ENCODER_MAIN);
//}
void __ISR(_CHANGE_NOTICE_VECTOR, IPL2AUTO) Main_encoder_Handler(void) {
    IFS1bits.CNIF = 0; // reset the flag
    if (PORTBbits.RB2 && !PORTBbits.RB3)
        event_handler(E_ENCODER_TURNED_RIGHT, E_SOURCE_ENCODER_MAIN);
    else
        event_handler(E_ENCODER_TURNED_LEFT, E_SOURCE_ENCODER_MAIN);
}
