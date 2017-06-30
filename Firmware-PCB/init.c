#include <xc.h>
#include "0x2a002a.h"



//extern char pattern[16][4][3];
//extern u8   I2C2_state;
//extern u8  SPI2_state;
//
//extern u32   bpm;
//
//extern u16 encoders_new_read;

void init_main_encoder(void)
{
    COD_A_GPIO = GPIO_INPUT;
    COD_B_GPIO = GPIO_INPUT;
}

void PPS_init(void)
{
//     #define PPSUnLock() {SYSKEY=0x0;SYSKEY=0xAA996655;SYSKEY=0x556699AA;CFGCONbits.IOLOCK=0;}
//#define PPSLock() {SYSKEY=0x0;SYSKEY=0xAA996655;SYSKEY=0x556699AA;CFGCONbits.IOLOCK=1;}
//    CFGCONbits.IOLOCK = 0;


    __builtin_disable_interrupts();

    SYSKEY=0x0;
    SYSKEY=0xAA996655;
    SYSKEY=0x556699AA;
    CFGCONbits.IOLOCK=0;
    SYSKEY=0x33333333;

    INT2R = 0b0100; // main encoder RPB2
    INT4R = 0b0001; // main encoder RPB3
	// RPA1 pour SDI1
	// RPB13 pour SDO1


//    CFGCONbits.IOLOCK = 1;
    SYSKEY=0x0;
    SYSKEY=0xAA996655;
    SYSKEY=0x556699AA;
    CFGCONbits.IOLOCK=1;
    SYSKEY=0x33333333;
}

void TIMER_init(void)
{
    TIMER3_STOP_AND_RESET;
    TIMER3_32_BITS_MODE = 1;
    TIMER3_VALUE = 0;
    set_bpm(bpm);

    TIMER5_STOP_AND_RESET;
    TIMER5_32_BITS_MODE = 1;
    TIMER5_VALUE = 0;
    TIMER5_PERIOD = FREQUENCY / (1000 / BUTTON_POLL_DELAY_MS) - 1;
    // y a t il besoin de 2 timers pour la gestion des appuis longs ?
    // on ne peut pas se debrouiller en divisant la clock ?
}

void INT_init(void)
{
    __builtin_disable_interrupts();
    INTCONbits.MVEC = 1;

//    // HT16 INTERRUPT 2
//    INTCONbits.INT2EP = FALLING_EDGE;
//    IFS0bits.INT2IF = 0; // Reset the flag
//    IPC2bits.INT2IP = 2; // Set priority
//    IEC0bits.INT2IE = 1; // Enable interrupt

     // MAIN ENCODER
     COD_A_INT_POLARITY = RISING_EDGE;
     COD_A_INT_FLAG_CLR;
     COD_A_INT_PRIORITY = 2;
     COD_A_INT_ENABLE = INT_ENABLED;
     COD_B_INT_POLARITY = RISING_EDGE;
     COD_B_INT_FLAG_CLR;
     COD_B_INT_PRIORITY = 2;
     COD_B_INT_ENABLE = INT_ENABLED;

    // TIMERS
    TIMER3_INT_FLAG_CLR;
    TIMER3_INT_PRIORITY = 3;
    TIMER3_INT_ENABLE = INT_ENABLED;
    TIMER5_INT_FLAG_CLR;
    TIMER5_INT_PRIORITY = 3;
    TIMER5_INT_ENABLE = INT_ENABLED;
    // l'interruption est activee uniquement en cas de poll

    //I2C1 INTERRUPT
	I2C1_INT_FLAG_CLR;
    I2C1_INT_PRIORITY = 4;
    I2C1_INT_ENABLE = 1;

//    //SPI2 Interrupt
//    IFS1bits.SPI2RXIF = 0; // reset flag
//    IFS1bits.SPI2TXIF = 0; // reset flag
//    IPC7bits.SPI2IP = 4; // au pif
//    IEC1bits.SPI2RXIE = 1;
//    IEC1bits.SPI2TXIE = 1;
//
//     // MCD encoders interrupts - pour le moment sur INT1 et INT2
//
//  //  TRISDbits.TRISD8 = 0x1;
//  //  TRISDbits.TRISD9 = 0x1;
//     INTCONbits.INT1EP = FALLING_EDGE;
//     IFS0bits.INT1IF = 0; // Reset the flag
//     IPC1bits.INT1IP = 2; // Set priority
//     IEC0bits.INT1IE = 1; // Enable interrupt
//
////     INTCONbits.INT2EP = FALLING_EDGE;
////     IFS0bits.INT2IF = 0; // Reset the flag
////     IPC2bits.INT2IP = 2; // Set priority
////     IEC0bits.INT2IE = 1; // Enable interrupt

    __builtin_enable_interrupts();
}

void SPI1_init(void)
{
	SPI1_CS0_GPIO = GPIO_OUTPUT;
	SPI1_CS1_GPIO = GPIO_OUTPUT;
	SPI1_CS2_GPIO = GPIO_OUTPUT;
	SPI1_CS3_GPIO = GPIO_OUTPUT;
	CS_MCP_LCD = CS_LINE_UP;
	CS_MCP_ENCODERS = CS_LINE_UP;
	CS_EEPROM = CS_LINE_UP;
	CS_SD = CS_LINE_UP;
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


void I2C1_init(void)
{
	u8	i = 0;
	u16	cpt;

	// envoi de 9 coups de clock pour reset les chips
	I2C1_PIN_GPIO = GPIO_OUTPUT;
	I2C1_PIN_LATCH = 0;
	while (i < 18)
	{
		I2C1_PIN_LATCH = !I2C1_PIN_LATCH;
		++i;
		cpt = 0;
		while (cpt++ < 2000);
	}
	I2C1BRG = 38;
	I2C1CONbits.DISSLW = 1;
	I2C1CONbits.STREN = 1;
	I2C1CONbits.RCEN = 1;
	I2C1CONbits.ON = 1;
}

//void HT16_init(void)
//{
//    u8  message[8] = {0};
//    int	i;
//    u8  config[5] = {
//        0x00,
//        0x21,
//        0xA1, // Int on falling edge
//        0xEF, // No dimming
//        0x81, // Blinking off display ON
//    };
//
//    i = 0;
//    while (i < sizeof(config) / sizeof(*config))
//    {
//        I2C2_write(0xE0, config[i], NULL, 0);
//        while (!(I2C2_state == E_I2C2_DONE))
//            WDTCONbits.WDTCLR = 1;
//        i++;
//    }
//
//    I2C2_write(0xE0, 0x00, message, 8);
//    while (!(I2C2_state == E_I2C2_DONE))
//        WDTCONbits.WDTCLR = 1; // CLEAR WATCHDOG
//
//    I2C2_read_callback(0xE0, 0x40, 6, NULL);
//    while (!(I2C2_state == E_I2C2_DONE))
//        WDTCONbits.WDTCLR = 1; // CLEAR WATCHDOG
//}
//
//void MCP_LCD_init(void)
//{
//        TRISEbits.TRISE2 = 0; //reset du MCP LCD // a degager quand branchement sur mclr
//
//        SPI2CONbits.MODE16 = 1;
//
//        //pins en output
//        SS_MCP_LCD = 0x0;
//        SPI2BUF = 0x4000;
//        while (SPI2STATbits.SPIBUSY) ;
//        SPI2BUF = 0x0000;
//        while (SPI2STATbits.SPIBUSY) ;
//        SS_MCP_LCD = 0x1;
//
//
//        //mode sequentiel off
//        SS_MCP_LCD = 0x0;
//        SPI2BUF = 0x400A;
//        while (SPI2STATbits.SPIBUSY) ;
//        SPI2BUF = 0x2020;
//        while (SPI2STATbits.SPIBUSY) ;
//        SS_MCP_LCD = 0x1;
//
//        SPI2CONbits.MODE16 = 0;
//}
//
//void MCP_ENCODERS_init(void)
//{
//
//    u16 read;
//
////        TRISEbits.TRISE2 = 0; //reset du MCP Encoders // a degager quand branchement sur mclr
////        TRISEbits.TRISE2 = 1; //reset du MCP Encoders // a degager quand branchement sur mclr
//        SPI2CONbits.MODE16 = 1;
//
//
////        //uniquement GPA0 et GPB 0 en input, reste en output
////        SS_MCP_ENCODERS = 0x0;
////        SPI2BUF = 0x4000;
////        while (SPI2STATbits.SPIBUSY) ;
////        SPI2BUF = 0x0101;
////        while (SPI2STATbits.SPIBUSY) ;
////        SS_MCP_ENCODERS = 0x1;
//
////        //mode sequentiel off
////        SS_MCP_ENCODERS = 0x0;
////        SPI2BUF = 0x400A;
////        while (SPI2STATbits.SPIBUSY) ;
////        SPI2BUF = 0x2020;
////        while (SPI2STATbits.SPIBUSY) ;
////        SS_MCP_ENCODERS = 0x1;
//
//
////        //pol inverse
////        SS_MCP_ENCODERS = 0x0;
////        SPI2BUF = 0x4002;
////        while (SPI2STATbits.SPIBUSY) ;
////        SPI2BUF = 0xFFFF;
////        while (SPI2STATbits.SPIBUSY) ;
////        SS_MCP_ENCODERS = 0x1;
//
////        //DEFVAL
////        SS_MCP_ENCODERS = 0x0;
////        SPI2BUF = 0x4006;
////        while (SPI2STATbits.SPIBUSY) ;
////        SPI2BUF = 0xFFFF;
////        while (SPI2STATbits.SPIBUSY) ;
////        SS_MCP_ENCODERS = 0x1;
////
////        //INTCON => 1 pour comparaison a DEFVAL, 0 pour comparaison a previous
////        SS_MCP_ENCODERS = 0x0;
////        SPI2BUF = 0x4008;
////        while (SPI2STATbits.SPIBUSY) ;
////        SPI2BUF = 0xFFFF;
////        while (SPI2STATbits.SPIBUSY) ;
////        SS_MCP_ENCODERS = 0x1;
//
////        //IOCON => le bit (1 << 1) est a 0 pour interrupt active low et 1 pour active high
////        //         le bit (1 << 6) active le mode mirroir
////        SS_MCP_ENCODERS = 0x0;
////        SPI2BUF = 0x400A;
////        while (SPI2STATbits.SPIBUSY) ;
////        SPI2BUF = 0x0000;
//////        SPI2BUF = 0x4040;
//////        SPI2BUF = 0x0202;
////        while (SPI2STATbits.SPIBUSY) ;
////        SS_MCP_ENCODERS = 0x1;
//
//        //read GPIO pour clear flags
//        SS_MCP_ENCODERS = 0x0;
//        SPI2BUF = 0x4110;
//        while (SPI2STATbits.SPIBUSY) ;
//        read = SPI2BUF;
//        SPI2BUF = 0x0000;
//        while (SPI2STATbits.SPIBUSY) ;
//        read = SPI2BUF;
//        //while (SPI2STATbits.SPIBUSY) ;
//        SS_MCP_ENCODERS = 0x1;
//
//
//        //activation des interrupts sur le port A
//        SS_MCP_ENCODERS = 0x0;
//        SPI2BUF = 0x4004;
////        while (SPI2STATbits.SPIBUSY) ;
//        while (SPI2STATbits.SPIBUSY) ;
//        read = SPI2BUF;
//        SPI2BUF = 0xFFFF;
//        while (SPI2STATbits.SPIBUSY) ;
//        read = SPI2BUF;
//        SS_MCP_ENCODERS = 0x1;
//
//        SPI2CONbits.MODE16 = 0;
//}

//void    LCD_init(void)
//{
//        u8  line = 0;
//        u8  col;
//
//        // clear du LCD
//        SS_MCP_LCD = 0x0;
//        SPI2BUF = 0x4012;
//        while (SPI2STATbits.SPIBUSY)
//            ;
//        LCD_blocking_control_instruction(1, 1, 0, 0, 0b00111111); //    LCD_display_on_off(1);
//        LCD_blocking_control_instruction(1, 1, 0, 0, 0b11000000); //    LCD_display_start_origin(0);
//        while (line < 8)
//        {
//            LCD_blocking_control_instruction(1, 1, 0, 0, 0b10111000 | line++);   //    LCD_display_set_x_page;
//            col = 0;
//            LCD_blocking_control_instruction(1, 1, 0, 0, 0b01000000 | col);      //    LCD_display_set_y_address(0);
//            while (col++ < 64)
//                LCD_blocking_control_instruction(1, 1, 1, 0, 0x00);
//        }
//        LCD_blocking_control_instruction(1, 1, 0, 0, 0b01000000); //    LCD_display_set_y_address(0);
//        LCD_blocking_control_instruction(1, 1, 0, 0, 0b10111000); //    LCD_display_set_x_page(0);
//        SS_MCP_LCD = 0x1;
//}


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
