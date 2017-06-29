#include <xc.h>
#include <sys/attribs.h>
#include "0x2a002a.h"

//extern u8 SPI_encoders_dirty;

////// Encodeur General OK !
void __ISR(_EXTERNAL_2_VECTOR, IPL2AUTO) Main_encoder_R_Handler(void) {
    //IFS0CLR = 0x800; // Clear INT3IF
    IFS0bits.INT2IF = 0;
    if (!PORTBbits.RB2 && PORTBbits.RB3)
        event_handler(E_ENCODER_TURNED_RIGHT, E_SOURCE_ENCODER_MAIN);

}

void __ISR(_EXTERNAL_4_VECTOR, IPL2AUTO) Main_encoder_L_Handler(void) {
    //IFS0CLR = 0x80000; // Clear INT4IF
    IFS0bits.INT4IF = 0;
    if (PORTBbits.RB2 && !PORTBbits.RB3)
        event_handler(E_ENCODER_TURNED_LEFT, E_SOURCE_ENCODER_MAIN);

}
//
//
////////Encodeurs via le MCP
//void __ISR(_EXTERNAL_1_VECTOR, IPL2AUTO) MCP_encoders_port_A_Handler(void) {
//     IFS0CLR = 0x80; // Clear INT1IF
//     SPI_encoders_dirty = 1;
//}