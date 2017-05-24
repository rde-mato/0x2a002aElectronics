#include <xc.h>
#include <sys/attribs.h>
#include "0x2a002a.h"

//////// Encodeur General OK !
//void __ISR(_EXTERNAL_3_VECTOR, IPL2AUTO) Main_encoder_R_Handler(void) {
//    IFS0CLR = 0x8000; // Clear INT3IF
//    if (PORTDbits.RD10 && !PORTDbits.RD11)
//        event_handler(E_ENCODER_TURNED_RIGHT, E_SOURCE_ENCODER_MAIN);
//
//}
//
//void __ISR(_EXTERNAL_4_VECTOR, IPL2AUTO) Main_encoder_L_Handler(void) {
//    IFS0CLR = 0x80000; // Clear INT4IF
//    if (!PORTDbits.RD10 && PORTDbits.RD11)
//        event_handler(E_ENCODER_TURNED_LEFT, E_SOURCE_ENCODER_MAIN);
//
//}


////Encodeurs via le MCP
void __ISR(_EXTERNAL_3_VECTOR, IPL2AUTO) Main_encoder_R_Handler(void) {
    IFS0CLR = 0x8000; // Clear INT3IF
///////faire un read du registre des int ... pour savoir quel encodeur
    ///// read des gpio mcp ....
    ///// suivre la logique suivante
    if (PORTDbits.RD10 && !PORTDbits.RD11)
        event_handler(E_ENCODER_TURNED_RIGHT, E_SOURCE_ENCODER_MAIN);

}

void __ISR(_EXTERNAL_4_VECTOR, IPL2AUTO) Main_encoder_L_Handler(void) {
    IFS0CLR = 0x80000; // Clear INT4IF

    if (!PORTDbits.RD10 && PORTDbits.RD11)
        event_handler(E_ENCODER_TURNED_LEFT, E_SOURCE_ENCODER_MAIN);

}