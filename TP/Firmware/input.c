#include <xc.h>
#include "tp.h"
#include <sys/attribs.h>

u8  ks_dirty = 0;

//extern void led_set(u8 index);

void __ISR(_EXTERNAL_2_VECTOR, IPL2AUTO) Int2Handler(void) {
    ks_dirty = 1;
    IFS0bits.INT2IF = 0; // Reset the flag
}