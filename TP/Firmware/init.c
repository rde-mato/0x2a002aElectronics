#include <xc.h>
#include "0x2a002a.h"

extern char pattern[16][4][3];
extern u8   I2C2_state;
extern u32   bpm;


void I2C2_init(void) //// A GERER AVEC DES INTERRUPTS
{
    u8 i;
    u32 cpt;
    // envoi de 9 coups de clock pour reset les chips
    TRISFbits.TRISF5 = 0x0; //F5 configured for output
    i = 0;
    LATFbits.LATF5 = 0;
    while (i < 18)
    {
        LATFbits.LATF5 = !LATFbits.LATF5;
        ++i;
        cpt = 0;
        while (cpt++ < 2000);
    }

    I2C2BRG = 38;
    I2C2CONbits.DISSLW = 1;
    I2C2CONbits.STREN = 1;
    I2C2CONbits.RCEN = 1;
    I2C2CONbits.ON = 1;
}

void HT16_init(void)
{
    u8  message[8] = {0};
    int	i;
    u8  config[5] = {
        0x00,
        0x21,
        0xA1, // Int on falling edge
        0xEF, // No dimming
        0x81, // Blinking off display ON
    };

    i = 0;
    while (i < sizeof(config) / sizeof(*config))
    {
        I2C2_write(0xE0, config[i], NULL, 0);
        while (!(I2C2_state == E_I2C2_DONE))
            WDTCONbits.WDTCLR = 1;
        i++;
    }

    I2C2_write(0xE0, 0x00, message, 8);
    while (!(I2C2_state == E_I2C2_DONE))
        WDTCONbits.WDTCLR = 1; // CLEAR WATCHDOG

    I2C2_read_callback(0xE0, 0x40, 6, NULL);
    while (!(I2C2_state == E_I2C2_DONE))
        WDTCONbits.WDTCLR = 1; // CLEAR WATCHDOG
}


void GPIO_init(void)
{
    u8 i;

    TRISFbits.TRISF1 = 0x0; //F1 configured for output
    TRISDbits.TRISD8 = 0x1; //D8 configured for input
    LED_ON_OFF = 0x0;

    // CodG GPIO RB2 & RB3
    TRISBbits.TRISB2 = 0x1; //configured for input
    TRISBbits.TRISB3 = 0x1; //configured for input
    CNCONbits.ON = 1; // enable change notification globally
    CNENbits.CNEN4 = 1; // enable change notification individually
    CNENbits.CNEN5 = 1; // enable change notification individually
    CNPUEbits.CNPUE4 = 0; //disable pull up a prionri pas necessaire
    CNPUEbits.CNPUE5 = 0; //disable pull up a prionri pas necessaire
    i = PORTBbits.RB2; //not sure, just to clear
    i = PORTBbits.RB3; //not sure, just to clear
}

void INT_init(void)
{
    __builtin_disable_interrupts();
    INTCONbits.MVEC = 1;

    // BUTTON INTERRUPT 1
    INTCONbits.INT1EP = 0; // Interrupt on falling edge
    IFS0bits.INT1IF = 0; // Reset the flag
    IPC1bits.INT1IP = 2; // Set priority
    IEC0bits.INT1IE = 1; // Enable interrupt
    
    // HT16 INTERRUPT 2
    INTCONbits.INT2EP = 0; // Interrupt on falling edge
    IFS0bits.INT2IF = 0; // Reset the flag
    IPC2bits.INT2IP = 2; // Set priority
    IEC0bits.INT2IE = 1; // Enable interrupt

    // Main encoder change notification interrupts
    IPC6bits.CNIP = 2; // Set priority
    IFS1bits.CNIF = 0; // reset the flag
    IEC1bits.CNIE = 1; //enable interrupt

    // TIMER 2-3 INTERRUPT
    IFS0bits.T3IF = 0;
    IPC3bits.T3IP = 3;
    IEC0bits.T3IE = 1;

    // TIMER 4-5 INTERRUPT
    IFS0bits.T5IF = 0;
    IPC5bits.T5IP = 2;
    IEC0bits.T5IE = 1;
    // l'interruption est activee uniquement en cas de poll

    //I2C2 INTERRUPT
    IFS1bits.I2C2MIF = 0; // reset flag
    IPC8bits.I2C2IP = 4; // au pif
    IEC1bits.I2C2MIE = 1;

    __builtin_enable_interrupts();
}

void TIMER_init(void)
{
    // timer 2-3 used for bpm management
    T2CON = 0x0; // Stop timer and clear control register
    T2CONbits.T32 = 0b1; // Activate 32bits timer with T3
    TMR2 = 0; // Clear timer register
    set_bpm(bpm);

    // timer 4-5 used for key press management
    T4CON = 0x0;
    T4CONbits.T32 = 0b1;
    TMR4 = 0;
    PR4 = FREQUENCY / (1000 / BUTTON_POLL_DELAY_MS) - 1;
}

void init_pattern(void)
{
    char i, j, k;

    i = j = k = -1;
    while (k++ < 16)
        while (j++ < 4)
            while(i++ < 3)
                pattern[k][j][i] = 0;
    pattern[0][0][notestatus] = 0b10010000;
    pattern[0][0][notevalue] = 50;
    pattern[0][0][notevelo] = 127;
    pattern[3][0][notestatus] = 0b10000000;
    pattern[3][0][notevalue] = 50;
    pattern[3][0][notevelo] = 127;

    pattern[2][0][notestatus] = 0b10010000;
    pattern[2][0][notevalue] = 52;
    pattern[2][0][notevelo] = 127;
    pattern[5][0][notestatus] = 0b10000000;
    pattern[5][0][notevalue] = 52;
    pattern[5][0][notevelo] = 127;

    pattern[4][0][notestatus] = 0b10010000;
    pattern[4][0][notevalue] = 50;
    pattern[4][0][notevelo] = 127;
    pattern[7][0][notestatus] = 0b10000000;
    pattern[7][0][notevalue] = 50;
    pattern[7][0][notevelo] = 127;
    pattern[4][1][notestatus] = 0b10010000;
    pattern[4][1][notevalue] = 51;
    pattern[4][1][notevelo] = 127;
    pattern[7][1][notestatus] = 0b10000000;
    pattern[7][1][notevalue] = 51;
    pattern[7][1][notevelo] = 127;

    pattern[6][0][notestatus] = 0b10010000;
    pattern[6][0][notevalue] = 52;
    pattern[6][0][notevelo] = 127;
    pattern[9][0][notestatus] = 0b10000000;
    pattern[9][0][notevalue] = 52;
    pattern[9][0][notevelo] = 127;

    pattern[8][0][notestatus] = 0b10010000;
    pattern[8][0][notevalue] = 50;
    pattern[8][0][notevelo] = 127;
    pattern[11][0][notestatus] = 0b10000000;
    pattern[11][0][notevalue] = 50;
    pattern[11][0][notevelo] = 127;

    pattern[10][0][notestatus] = 0b10010000;
    pattern[10][0][notevalue] = 52;
    pattern[10][0][notevelo] = 127;
    pattern[13][0][notestatus] = 0b10000000;
    pattern[13][0][notevalue] = 52;
    pattern[13][0][notevelo] = 127;

    pattern[12][0][notestatus] = 0b10010000;
    pattern[12][0][notevalue] = 50;
    pattern[12][0][notevelo] = 127;
    pattern[15][0][notestatus] = 0b10000000;
    pattern[15][0][notevalue] = 50;
    pattern[15][0][notevelo] = 127;
    pattern[12][1][notestatus] = 0b10010000;
    pattern[12][1][notevalue] = 51;
    pattern[12][1][notevelo] = 127;
    pattern[15][1][notestatus] = 0b10000000;
    pattern[15][1][notevalue] = 51;
    pattern[15][1][notevelo] = 127;

    pattern[14][0][notestatus] = 0b10010000;
    pattern[14][0][notevalue] = 52;
    pattern[14][0][notevelo] = 127;
    pattern[1][0][notestatus] = 0b10000000;
    pattern[1][0][notevalue] = 52;
    pattern[1][0][notevelo] = 127;
}