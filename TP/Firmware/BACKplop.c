//#include <xc.h>
//#include "tp.h"
//#include <sys/attribs.h>
//
//#define LED_ON_OFF LATFbits.LATF1
//#define BUTTON_SWITCH_OPEN PORTDbits.RD8
//#define F1_IO_SELECTOR TRISFbits.TRISF1
//#define D8_IO_SELECTOR TRISDbits.TRISD8
//#define FREQUENCY (8000000ul)
//#define PBCLK 8000000
//#define MIDI_BAUD_RATE 31250
//#define SET_MIDI_BRG ((u16)(PBCLK / (16 * MIDI_BAUD_RATE) - 1))
//
//u8 twice_hz = 1;
//u8 dim_mode = 0;
//s8 dim_edge = 1;
//u8 pwm_high;
//u16 pwm_width;
//
//void __ISR(_EXTERNAL_1_VECTOR, IPL2AUTO) Int1Handler(void) {
//    if (!INTCONbits.INT1EP) {
//        TMR2 = 0;
//        //TMR3 = 0;
//        T2CONbits.ON = 1;
//        //T3CONbits.ON = 1;
//        TMR4 = 0;
//        //TMR5 = 0;
//        if (!dim_mode) {
//            twice_hz = (twice_hz == 16) ? 1 : twice_hz * 2;
//            PR4 = FREQUENCY / twice_hz;
//        }
//    } else {
//        T2CONbits.ON = 0;
//        //T3CONbits.ON = 0;
//    }
//    INTCONbits.INT1EP = !INTCONbits.INT1EP;
//    IFS0bits.INT1IF = 0; // Reset the flag
//}
//
//void __ISR(_TIMER_1_VECTOR, IPL4AUTO) Timer1Handler(void) {
//    pwm_width = dim_edge ? pwm_width + 1 : pwm_width - 1;
//    if (pwm_width == 0 || pwm_width == 15999)
//        dim_edge = !dim_edge;
//    IFS0bits.T1IF = 0; // Reset the flag
//}
//
//void __ISR(_TIMER_3_VECTOR, IPL3AUTO) Timer3Handler(void) {
//    dim_mode = !dim_mode;
//    pwm_high = 0;
//    pwm_width = 100;
//    IFS0bits.T3IF = 0; // Reset the flag
//    if (dim_mode)
//        IFS0bits.T5IF = 1; // Set the flag
//    else {
//        PR4 = FREQUENCY / twice_hz;
//        TMR4 = 0;
//    }
//}
//
//static void send_msg(void) {
//    //U2TXREG = 0b01010101;
//
//    if (LED_ON_OFF)
//        U2TXREG = 0b10010000; // note on sur channel 0
//    else
//        U2TXREG = 0b10000000; // note off sur channel 0
//    U2TXREG = 0x42;
//    U2TXREG = 0x42;
//
//}
//
//void __ISR(_TIMER_5_VECTOR, IPL3AUTO) Timer5Handler(void) {
//    send_msg();
//    LED_ON_OFF = !LED_ON_OFF;
//    if (dim_mode) {
//        PR4 = pwm_high ? pwm_width + 1 : 16000 - pwm_width;
//        TMR4 = 0;
//        pwm_high = !pwm_high;
//    }
//    IFS0bits.T5IF = 0;
//}
//
//static void init_gpios(void) {
//    F1_IO_SELECTOR = 0x0; //F1 configured for output
//    D8_IO_SELECTOR = 0x1; //D8 configured for input
//    LED_ON_OFF = 0x0;
//}
//
//static void init_interrupts(void) {
//    __builtin_disable_interrupts();
//    INTCONbits.MVEC = 1;
//
//    // BUTTON INTERRUPT 1
//    INTCONbits.INT1EP = 0; // Interrupt on falling edge
//    IFS0bits.INT1IF = 0; // Reset the flag
//    IPC1bits.INT1IP = 2; // Set priority
//    IEC0bits.INT1IE = 1; // Enable interrupt
//
//    // TIMER 1 INTERRUPT
//    IFS0bits.T1IF = 0;
//    IPC1bits.T1IP = 4;
//    IEC0bits.T1IE = 1;
//
//    // TIMER 2-3 INTERRUPT
//    IFS0bits.T3IF = 0;
//    IPC3bits.T3IP = 3;
//    IEC0bits.T3IE = 1;
//
//    // TIMER 4-5 INTERRUPT
//    IFS0bits.T5IF = 0;
//    IPC5bits.T5IP = 3;
//    IEC0bits.T5IE = 1;
//
//    __builtin_enable_interrupts();
//}
//
//static void init_timer(void) {
//    T1CON = 0x0; // Stop timer and clear control register
//    TMR1 = 0; // Clear timer register
//    T1CONbits.TCKPS = 0; //b11;	// set prescaler to 256
//    PR1 = FREQUENCY / 16000 * 5; // interrupt 254 times every 5 secs
//    T2CON = 0x0; // Stop timer and clear control register
//    T2CONbits.T32 = 0b1; // Activate 32bits timer with T3
//    TMR2 = 0; // Clear timer register
//    //TMR3 = 0;		// Clear timer register
//    T4CON = 0x0; // Stop timer and clear control register
//    T4CONbits.T32 = 0b1; // Activate 32bits timer with T3
//    TMR4 = 0; // Clear timer register
//    //TMR5 = 0;		// Clear timer register
//}
//
//static void init_uart(void) {
//    //U2MODE = 0;
//    U2MODEbits.ON = 0;
//
//    U2MODEbits.BRGH = 0;
//        U2BRG = SET_MIDI_BRG;
//
//    U2MODEbits.UEN = 0b00;
//
//    U2STAbits.UTXEN = 1;
//    U2STAbits.URXEN = 0;
//    U2MODEbits.ON = 1;
//
//}
//
////int main1 (void)
////{
////		u8	last;
////		u8	new;
////
////		last = BUTTON_SWITCH_OPEN;
////		new = BUTTON_SWITCH_OPEN;
////		init_gpios();
////		while (42)
////		{
////				last = new;
////				new = BUTTON_SWITCH_OPEN;
////				if (last && !new)
////						LED_ON_OFF = !LED_ON_OFF;
////		}
////		return (0);
////}
////
////int main2 (void)
////{
////		u8	new;
////		u8	last;
////		u32	ticks;
////
////		init_gpios();
////		init_timer();
////		new = BUTTON_SWITCH_OPEN;
////		twice_hz = 1;
////		ticks = FREQUENCY / twice_hz;
////		T4CONbits.ON = 1;
////		T5CONbits.ON = 1;
////		while (42)
////		{
////				last = new;
////				new = BUTTON_SWITCH_OPEN;
////				if (last && !new)
////				{
////						twice_hz = (twice_hz == 16) ? 1 : twice_hz * 2;
////						ticks = FREQUENCY / twice_hz;
////						TMR4 %= ticks;
////				}
////				if (TMR4 >= ticks)
////				{
////						LED_ON_OFF = !LED_ON_OFF;
////						TMR4 %= ticks;
////				}
////		}
////		return (0);
////}
//
//int main(void) {
//    init_gpios();
//    init_interrupts();
//    init_timer();
//    init_uart();
//    PR2 = 2 * FREQUENCY;
//    PR4 = FREQUENCY / twice_hz;
//    T1CONbits.ON = 1;
//    T4CONbits.ON = 1;
//    //T5CONbits.ON = 1;
//    while (42)
//        WDTCONbits.WDTCLR = 1; // CLEAR WATCHDOG
//    return (0);
//}
