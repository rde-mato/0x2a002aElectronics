#include <xc.h>
#include <sys/attribs.h>
#include "0x2a002a.h"

extern u8 SPI_encoders_dirty;

void __ISR(_EXTERNAL_2_VECTOR, IPL2AUTO) Main_encoder_A_Handler(void) {
    COD_A_INT_FLAG_CLR;
    if (PORTBbits.RB2 && !PORTBbits.RB3)
        event_handler(E_ENCODER_TURNED_RIGHT, E_SOURCE_ENCODER_MAIN);

}

void __ISR(_EXTERNAL_4_VECTOR, IPL2AUTO) Main_encoder_B_Handler(void) {
    COD_B_INT_FLAG_CLR;
    if (!PORTBbits.RB2 && PORTBbits.RB3)
        event_handler(E_ENCODER_TURNED_LEFT, E_SOURCE_ENCODER_MAIN);

}

void __ISR(_EXTERNAL_3_VECTOR, IPL2AUTO) MCP_encoders_port_B_Handler(void) {
     IFS0CLR = (1 << 18);
     SPI_encoders_dirty = 1;
}