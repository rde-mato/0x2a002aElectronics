#include <xc.h>
#include "0x2a002a.h"

extern char pattern[16][4][3];
extern u8   I2C2_state;
extern u8  SPI2_state;

extern u32   bpm;

void SPI2_init(void)
{
    __builtin_disable_interrupts();

    SPI2CON = 0;
    SPI2BUF = 0;
    SPI2BRG = 0; //set baudrate 1Mhz suivant 8 Mhz du pbclk
   // SPI2STATbits.SPIROV = 0;
    SPI2CONbits.CKE = 1;
    SPI2CONbits.CKP = 0; // mode 00 tás vue
   // SPI2CONbits.MODE16 = 0;
    SPI2CONbits.MODE32 = 1;
    SPI2CONbits.MSTEN = 1; //activer master mode
    SPI2CONbits.ON = 1; //ON spi2

    __builtin_enable_interrupts();
}


void I2C2_init(void) //// A GERER AVEC DES INTERRUPTS
{
    u8 i;
    u16 cpt;
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

//u32 SPI2_transmit32bits(u32 msg) //ancienne version du transmit 32 bits
//{
//    u32 res;
//
//    SPI2CONbits.MODE32 = 1;
//    LATGbits.LATG9 = 0x0; //SS a 0
//    SPI2BUF = msg;
//    while (!SPI2STATbits.SPIRBF) ;
//    res = SPI2BUF;
//    LATGbits.LATG9 = 0x1; //SS a 1
//    return (res);
//}

//void MCP_init_encoders(void)
//{
//    SPI2_transmit32bits(0x4004FFFF); // active les interrupts sur tous les ports
//}

//void MCP_init_encoders(void)
//{
//    u32 res;
//
//    SPI2CONbits.MODE32 = 1;
//    LATGbits.LATG9 = 0x0; //SS a 0
//    SPI2BUF = 0x4004FFFF;
//    while (!SPI2STATbits.SPIRBF) ;
//    res = SPI2BUF;
//    LATGbits.LATG9 = 0x1; //SS a 1
//    SPI2CONbits.MODE32 = 0; // on remet a zero car defaut en 8 bits
//    return (res);
//}

//void MCP_init_LCD(void)
//{
//    SPI2_transmit32bits(0x40000000); // fout toutes les GPIO en output
//}

void MCP_init_LCD(void)
{
    u32 res;

    SPI2CONbits.MODE32 = 1;
    LATGbits.LATG9 = 0x0; //SS a 0
    SPI2BUF = 0x40000000;
    while (!SPI2STATbits.SPIRBF) ;
    res = SPI2BUF;
    LATGbits.LATG9 = 0x1; //SS a 1
    SPI2CONbits.MODE32 = 0; // on remet a zero car defaut en 8 bits
    return (res);
}

//void MCP_init_LCD(void)
//{
//    SPI2_push(E_SPI2_SS_MCP_LCD, 0x40);
//    SPI2_push(E_SPI2_SS_MCP_LCD, 0x00);
//    SPI2_push(E_SPI2_SS_MCP_LCD, 0x00);
//    SPI2_push(E_SPI2_SS_MCP_LCD, 0x00);
//    SPI2_state = E_SPI2_CONFIG;     // idealement a degager
//    IFS1bits.SPI2RXIF = 1;
//}


void GPIO_init(void)
{
    TRISFbits.TRISF1 = 0x0; //F1 configured for output
    TRISDbits.TRISD8 = 0x1; //D8 configured for input
    LED_ON_OFF = 0x0;

    // CodG GPIO D11
    TRISDbits.TRISD10 = 0x1; //D10 configured for input
    TRISDbits.TRISD11 = 0x1; //D10 configured for input

    //GPIO pour le SPI2
    TRISGbits.TRISG7 = 0x1;
    TRISGbits.TRISG8 = 0x0;
    //SS di MCP bordel.... a arranger apres quand on qurq plusieur chip spi
    TRISGbits.TRISG9 = 0x0;
    LATGbits.LATG9 = 0x1;
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

     // CodG INTERRUPT 3
     INTCONbits.INT3EP = 1; // Interrupt on rising edge
     IFS0bits.INT3IF = 0; // Reset the flag
     IPC3bits.INT3IP = 2; // Set priority
     IEC0bits.INT3IE = 1; // Enable interrupt

     // CodG INTERRUPT 4
     INTCONbits.INT4EP = 1; // Interrupt on rising edge
     IFS0bits.INT4IF = 0; // Reset the flag
     IPC4bits.INT4IP = 2; // Set priority
     IEC0bits.INT4IE = 1; // Enable interrupt

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

    //SPI2 Interrupt
    IFS1bits.SPI2RXIF = 0; // reset flag
    IFS1bits.SPI2TXIF = 0; // reset flag
    IPC7bits.SPI2IP = 4; // au pif
    IEC1bits.SPI2RXIE = 1;
    IEC1bits.SPI2TXIE = 1;

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