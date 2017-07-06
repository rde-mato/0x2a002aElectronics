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
    TIMER2_STOP_AND_RESET;
    TIMER2_VALUE = 0;
    TIMER2_PRESCALE = TIMER_PRESCALE_256;

    TIMER3_STOP_AND_RESET;
    TIMER3_VALUE = 0;
    TIMER3_PRESCALE = TIMER_PRESCALE_256;
    TIMER3_PERIOD = 0xFFFF;

    TIMER4_STOP_AND_RESET;
    TIMER4_VALUE = 0;
    TIMER4_PRESCALE = TIMER_PRESCALE_256;
    TIMER4_PERIOD = 60000;
//	TIMER4_PERIOD = ONE_MILLISECOND / 64 * SCREEN_DURATION_MS - 1;

    TIMER5_STOP_AND_RESET;
    TIMER5_VALUE = 0;
    TIMER5_PRESCALE = TIMER_PRESCALE_4;
    TIMER5_PERIOD = 64 * FREQUENCY / (8000 / BUTTON_POLL_DELAY_MS) - 1;
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

    // TIMERS
    TIMER2_INT_FLAG_CLR;
    TIMER2_INT_PRIORITY = 3;
    TIMER2_INT_ENABLE = INT_ENABLED;
    TIMER3_INT_FLAG_CLR;
    TIMER3_INT_PRIORITY = 1;
    TIMER3_INT_ENABLE = INT_ENABLED;
    TIMER4_INT_FLAG_CLR;
    TIMER4_INT_PRIORITY = 1;
    TIMER4_INT_ENABLE = INT_ENABLED;
    TIMER5_INT_FLAG_CLR;
    TIMER5_INT_PRIORITY = 3;
    TIMER5_INT_ENABLE = INT_ENABLED;
    // l'interruption est activee uniquement en cas de poll

    __builtin_enable_interrupts();
}