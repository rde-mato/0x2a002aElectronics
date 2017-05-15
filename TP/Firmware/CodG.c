#include <xc.h>
#include <sys/attribs.h>
#include "0x2a002a.h"

u8 index = 0;

void __ISR(_EXTERNAL_3_VECTOR, IPL2AUTO) CodG_R_Handler(void) {
    IFS0CLR = 0x8000; // Clear INT3IF
    led_toggle(index);
    PORTDbits.RD11 ? --index : ++index;
    index &= 0x0F;
    led_toggle(index);
}