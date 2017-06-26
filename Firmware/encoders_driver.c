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


//////Encodeurs via le MCP
void __ISR(_EXTERNAL_1_VECTOR, IPL2AUTO) MCP_encoders_port_A_Handler(void) {

    u16 read;
    u8 flags_A;
    u8 intcap_A;
    u8 intcap_B;
    
     IFS0CLR = 0x80; // Clear INT1IF //IFS0bits.INT1IF = 0;
     SPI2STATCLR=0x40;// clear the Overflow
    //read des interrupts flags INTCAP

//    if (PORTDbits.RD8 && !PORTDbits.RD9)
//        event_handler(E_ENCODER_TURNED_RIGHT, E_SOURCE_ENCODER_MAIN);

     
    SPI2CONbits.MODE16 = 1;

    SS_MCP_ENCODERS = 0x0;
    SPI2BUF = 0x410E;
    while (SPI2STATbits.SPIBUSY) ;
    read = SPI2BUF;
    SPI2BUF = 0x0000;
    while (SPI2STATbits.SPIBUSY) ;
    read = SPI2BUF;
    flags_A = read >> 8;
    SS_MCP_ENCODERS = 0x1;

    SS_MCP_ENCODERS = 0x0;
    SPI2BUF = 0x4110;
    while (SPI2STATbits.SPIBUSY) ;
    read = SPI2BUF;
    SPI2BUF = 0x0000;
    while (SPI2STATbits.SPIBUSY) ;
    read = SPI2BUF;
    SS_MCP_ENCODERS = 0x1;

    
    intcap_A = (u8)(read >> 8);
    intcap_B = (u8)read;

    u8 i = 0;
    while (i < 8)
    {
        if (flags_A & (1 << i))
            event_handler((intcap_A & (1 << i)) != (intcap_B & (1 << i)) ? E_ENCODER_TURNED_RIGHT : E_ENCODER_TURNED_LEFT, i);
        ++i;
    }

    SPI2CONbits.MODE16 = 0;

}

//void __ISR(_EXTERNAL_2_VECTOR, IPL2AUTO) MCP_encoders_port_B_Handler(void) {
//
//    //IFS0bits.INT2IF = 0;
//    //IFS0CLR = 0x800; // Clear INT2IF
//            //read des interrupts flags INTCAP
//    if (!PORTDbits.RD8 && PORTDbits.RD9)
//        event_handler(E_ENCODER_TURNED_LEFT, E_SOURCE_ENCODER_MAIN);
//    SPI2CONbits.MODE16 = 1;
//    SS_MCP_ENCODERS = 0x0;
//    SPI2BUF = 0x4110;
//    while (SPI2STATbits.SPIBUSY) ;
//    SPI2BUF = 0x0000;
//    while (SPI2STATbits.SPIBUSY) ;
//    SS_MCP_ENCODERS = 0x1;
//    SPI2CONbits.MODE16 = 0;
//
//        IFS0CLR = 0x800; // Clear INT2IF
//}