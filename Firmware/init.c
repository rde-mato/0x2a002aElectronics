#include <xc.h>
#include "0x2a002a.h"


#define RISING_EDGE    1
#define FALLING_EDGE    0

extern char pattern[16][4][3];
extern u8   I2C2_state;
extern u8  SPI2_state;

extern u32   bpm;

void SPI2_init(void)
{

    //GPIO pour le SPI2
    TRISGbits.TRISG7 = 0x1;
    TRISGbits.TRISG8 = 0x0;

    //Slave select en mode test, a adapter avec vrais branchements
    TRISGbits.TRISG9 = 0x0; //chip select du MCP LCD
    LATGbits.LATG9 = 0x1;

    SPI2CON = 0;
    SPI2BUF = 0;
    SPI2BRG = 0; //set baudrate 1Mhz suivant 8 Mhz du pbclk
    SPI2CONbits.CKE = 1;
    SPI2CONbits.CKP = 0; // mode 00 tás vue
    SPI2CONbits.MODE16 = 0;
    SPI2CONbits.MODE32 = 0;
    SPI2CONbits.MSTEN = 1; //activer master mode
    SPI2CONbits.ON = 1; //ON spi2
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

void MCP_LCD_init(void)
{
        TRISEbits.TRISE2 = 0; //reset du MCP LCD // a degager quand branchement sur mclr
        
        SPI2CONbits.MODE16 = 1;

        //pins en output
        SS_MCP_LCD = 0x0;
        SPI2BUF = 0x4000;
        while (!SPI2STATbits.SPIRBF) ;
        SPI2BUF = 0x0000;
        while (!SPI2STATbits.SPIRBF) ;
        SS_MCP_LCD = 0x1;


        //mode sequentiel off
        SS_MCP_LCD = 0x0;
        SPI2BUF = 0x400A;
        while (!SPI2STATbits.SPIRBF) ;
        SPI2BUF = 0x2020;
        while (!SPI2STATbits.SPIRBF) ;
        SS_MCP_LCD = 0x1;

        SPI2CONbits.MODE16 = 0;
}

void MCP_ENCODERS_init(void)
{
//        TRISEbits.TRISE2 = 0; //reset du MCP Encoders // a degager quand branchement sur mclr
//        TRISEbits.TRISE2 = 1; //reset du MCP Encoders // a degager quand branchement sur mclr
        SPI2CONbits.MODE16 = 1;


//        //mode sequentiel off
//        SS_MCP_ENCODERS = 0x0;
//        SPI2BUF = 0x400A;
//        while (SPI2STATbits.SPIBUSY) ;
//        SPI2BUF = 0x2020;
//        while (SPI2STATbits.SPIBUSY) ;
//        SS_MCP_ENCODERS = 0x1;

        //activation des interrupts
        SS_MCP_ENCODERS = 0x0;
        SPI2BUF = 0x4004;
        while (!SPI2STATbits.SPIRBF) ;
        SPI2BUF = 0xFFFF;
        while (!SPI2STATbits.SPIRBF) ;
        SS_MCP_ENCODERS = 0x1;

//        //pol inverse
//        SS_MCP_ENCODERS = 0x0;
//        SPI2BUF = 0x4002;
//        while (!SPI2STATbits.SPIRBF) ;
//        SPI2BUF = 0xFFFF;
//        while (!SPI2STATbits.SPIRBF) ;
//        SS_MCP_ENCODERS = 0x1;
//
//        //DEFVAL
//        SS_MCP_ENCODERS = 0x0;
//        SPI2BUF = 0x4006;
//        while (!SPI2STATbits.SPIRBF) ;
//        SPI2BUF = 0xFFFF;
//        while (!SPI2STATbits.SPIRBF) ;
//        SS_MCP_ENCODERS = 0x1;

        //INTCON
        SS_MCP_ENCODERS = 0x0;
        SPI2BUF = 0x4008;
        while (!SPI2STATbits.SPIRBF) ;
        SPI2BUF = 0x0000;
        while (!SPI2STATbits.SPIRBF) ;
        SS_MCP_ENCODERS = 0x1;

        //IOCON
        SS_MCP_ENCODERS = 0x0;
        SPI2BUF = 0x400A;
        while (!SPI2STATbits.SPIRBF) ;
        SPI2BUF = 0x0202;
        while (!SPI2STATbits.SPIRBF) ;
        SS_MCP_ENCODERS = 0x1;

        //read GPIO pour clear flags
        SS_MCP_ENCODERS = 0x0;
        SPI2BUF = 0x4112;
        while (!SPI2STATbits.SPIRBF) ;
        SPI2BUF = 0x0000;
        while (!SPI2STATbits.SPIRBF) ;
        SS_MCP_ENCODERS = 0x1;


        SPI2CONbits.MODE16 = 0;
}

void    LCD_init(void)
{
        u8  line = 0;
        u8  col;

        // clear du LCD
        SS_MCP_LCD = 0x0;
        SPI2BUF = 0x4012;
        while (!SPI2STATbits.SPIRBF)
            ;
        LCD_blocking_control_instruction(1, 1, 0, 0, 0b00111111); //    LCD_display_on_off(1);
        LCD_blocking_control_instruction(1, 1, 0, 0, 0b11000000); //    LCD_display_start_origin(0);
        while (line < 8)
        {
            LCD_blocking_control_instruction(1, 1, 0, 0, 0b10111000 | line++);   //    LCD_display_set_x_page;
            col = 0;
            LCD_blocking_control_instruction(1, 1, 0, 0, 0b01000000 | col);      //    LCD_display_set_y_address(0);
            while (col++ < 64)
                LCD_blocking_control_instruction(1, 1, 1, 0, 0x00);
        }
        LCD_blocking_control_instruction(1, 1, 0, 0, 0b01000000); //    LCD_display_set_y_address(0);
        LCD_blocking_control_instruction(1, 1, 0, 0, 0b10111000); //    LCD_display_set_x_page(0);
        SS_MCP_LCD = 0x1;
}

void GPIO_init(void)
{
    TRISFbits.TRISF1 = 0x0; //F1 configured for output
    TRISDbits.TRISD8 = 0x1; //D8 configured for input
    LED_ON_OFF = 0x0;

    // CodG GPIO D11
    TRISDbits.TRISD10 = 0x1; //D10 configured for input
    TRISDbits.TRISD11 = 0x1; //D10 configured for input
}

void INT_init(void)
{
    __builtin_disable_interrupts();
    INTCONbits.MVEC = 1;

    
//    // HT16 INTERRUPT 2
//    INTCONbits.INT2EP = 0; // Interrupt on falling edge
//    IFS0bits.INT2IF = 0; // Reset the flag
//    IPC2bits.INT2IP = 2; // Set priority
//    IEC0bits.INT2IE = 1; // Enable interrupt

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

     // MCD encoders interrupts - pour le moment sur INT1 et INT2
        
  //  TRISDbits.TRISD8 = 0x1;
  //  TRISDbits.TRISD9 = 0x1;
     INTCONbits.INT1EP = RISING_EDGE;
     IFS0bits.INT1IF = 0; // Reset the flag
     IPC1bits.INT1IP = 2; // Set priority
     IEC0bits.INT1IE = 1; // Enable interrupt
     
     INTCONbits.INT2EP = RISING_EDGE;
     IFS0bits.INT2IF = 0; // Reset the flag
     IPC2bits.INT2IP = 2; // Set priority
     IEC0bits.INT2IE = 1; // Enable interrupt

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

//void init_pattern(void)
//{
//    char i, j, k;
//
//    i = j = k = -1;
//    while (k++ < 16)
//        while (j++ < 4)
//            while(i++ < 3)
//                pattern[k][j][i] = 0;
//    pattern[0][0][notestatus] = 0b10010000;
//    pattern[0][0][notevalue] = 50;
//    pattern[0][0][notevelo] = 127;
//    pattern[3][0][notestatus] = 0b10000000;
//    pattern[3][0][notevalue] = 50;
//    pattern[3][0][notevelo] = 127;
//
//    pattern[2][0][notestatus] = 0b10010000;
//    pattern[2][0][notevalue] = 52;
//    pattern[2][0][notevelo] = 127;
//    pattern[5][0][notestatus] = 0b10000000;
//    pattern[5][0][notevalue] = 52;
//    pattern[5][0][notevelo] = 127;
//
//    pattern[4][0][notestatus] = 0b10010000;
//    pattern[4][0][notevalue] = 50;
//    pattern[4][0][notevelo] = 127;
//    pattern[7][0][notestatus] = 0b10000000;
//    pattern[7][0][notevalue] = 50;
//    pattern[7][0][notevelo] = 127;
//    pattern[4][1][notestatus] = 0b10010000;
//    pattern[4][1][notevalue] = 51;
//    pattern[4][1][notevelo] = 127;
//    pattern[7][1][notestatus] = 0b10000000;
//    pattern[7][1][notevalue] = 51;
//    pattern[7][1][notevelo] = 127;
//
//    pattern[6][0][notestatus] = 0b10010000;
//    pattern[6][0][notevalue] = 52;
//    pattern[6][0][notevelo] = 127;
//    pattern[9][0][notestatus] = 0b10000000;
//    pattern[9][0][notevalue] = 52;
//    pattern[9][0][notevelo] = 127;
//
//    pattern[8][0][notestatus] = 0b10010000;
//    pattern[8][0][notevalue] = 50;
//    pattern[8][0][notevelo] = 127;
//    pattern[11][0][notestatus] = 0b10000000;
//    pattern[11][0][notevalue] = 50;
//    pattern[11][0][notevelo] = 127;
//
//    pattern[10][0][notestatus] = 0b10010000;
//    pattern[10][0][notevalue] = 52;
//    pattern[10][0][notevelo] = 127;
//    pattern[13][0][notestatus] = 0b10000000;
//    pattern[13][0][notevalue] = 52;
//    pattern[13][0][notevelo] = 127;
//
//    pattern[12][0][notestatus] = 0b10010000;
//    pattern[12][0][notevalue] = 50;
//    pattern[12][0][notevelo] = 127;
//    pattern[15][0][notestatus] = 0b10000000;
//    pattern[15][0][notevalue] = 50;
//    pattern[15][0][notevelo] = 127;
//    pattern[12][1][notestatus] = 0b10010000;
//    pattern[12][1][notevalue] = 51;
//    pattern[12][1][notevelo] = 127;
//    pattern[15][1][notestatus] = 0b10000000;
//    pattern[15][1][notevalue] = 51;
//    pattern[15][1][notevelo] = 127;
//
//    pattern[14][0][notestatus] = 0b10010000;
//    pattern[14][0][notevalue] = 52;
//    pattern[14][0][notevelo] = 127;
//    pattern[1][0][notestatus] = 0b10000000;
//    pattern[1][0][notevalue] = 52;
//    pattern[1][0][notevelo] = 127;
//}