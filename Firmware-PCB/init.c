#include <xc.h>
#include "0x2a002a.h"

void GPIO_and_PPS_init(void)
{
    __builtin_disable_interrupts();

    SYSKEY = 0x0;
    SYSKEY = 0xAA996655;
    SYSKEY = 0x556699AA;
    CFGCONbits.IOLOCK = 0;
    SYSKEY = 0x33333333;

    pps_init_encoders();
    pps_init_SPI1();
    pps_init_UART();

    SYSKEY = 0x0;
    SYSKEY = 0xAA996655;
    SYSKEY = 0x556699AA;
    CFGCONbits.IOLOCK = 1;
    SYSKEY = 0x33333333;
}

void TIMER_init(void)
{
    timer_1_init();
    timer_2_init();
    timer_3_init();
    timer_4_init();
    timer_5_init();
}

void INT_init(void)
{
    __builtin_disable_interrupts();
    INTCONbits.MVEC = 1;

    int_init_HT16_press();
    int_init_main_encoder();
    int_init_UART();
    int_init_I2C1();
    int_init_SPI1();
    int_init_MCP_encoders();
    int_init_timer2();
    int_init_timer3();
    int_init_timer4();
    int_init_timer5();

    __builtin_enable_interrupts();
}