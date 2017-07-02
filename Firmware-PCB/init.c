#include <xc.h>
#include "0x2a002a.h"



//extern char pattern[16][4][3];
extern u8   I2C1_state;
//extern u8  SPI1_state;

extern u32   bpm;
//
//extern u16 encoders_new_read;

void GPIO_and_PPS_init(void)
{
    __builtin_disable_interrupts();

    SYSKEY = 0x0;
    SYSKEY = 0xAA996655;
    SYSKEY = 0x556699AA;
    CFGCONbits.IOLOCK = 0;
    SYSKEY = 0x33333333;

	// Main encoder A
	COD_A_ANALOG = DIGITAL_PIN;
	COD_A_GPIO = GPIO_INPUT;
	PPS_COD_A;

	// Main encoder B
	COD_B_ANALOG = DIGITAL_PIN;
	COD_B_GPIO = GPIO_INPUT;
	PPS_COD_B;

	// SPI1
	SPI1_CS0_GPIO = GPIO_OUTPUT;
	CS_MCP_LCD = CS_LINE_UP;
	SPI1_CS1_GPIO = GPIO_OUTPUT;
	CS_MCP_ENCODERS = CS_LINE_UP;
	SPI1_CS2_GPIO = GPIO_OUTPUT;
	CS_EEPROM = CS_LINE_UP;
	SPI1_CS3_GPIO = GPIO_OUTPUT;
	CS_SD = CS_LINE_UP;
        SPI1_CS2_ANALOG = DIGITAL_PIN;
        SPI1_CS3_ANALOG = DIGITAL_PIN;
	SDI1_ANALOG = DIGITAL_PIN;
	PPS_SDI1;
	SDO1_ANALOG = DIGITAL_PIN;
	PPS_SDO1;
        MIDI_ANALOG = DIGITAL_PIN;
        PPS_MIDI;
        MCP_ENC_A_ANALOG = DIGITAL_PIN;
        COD_MCP_A_GPIO = GPIO_INPUT;
        PPS_MCP_ENC_A;
        MCP_ENC_B_ANALOG = DIGITAL_PIN;
        COD_MCP_B_GPIO = GPIO_INPUT;
        PPS_MCP_ENC_B;

	SYSKEY = 0x0;
	SYSKEY = 0xAA996655;
	SYSKEY = 0x556699AA;
	CFGCONbits.IOLOCK = 1;
	SYSKEY = 0x33333333;
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
	TIMER5_PERIOD = FREQUENCY / (1000 / LONG_PRESS_LIMIT) - 1;
	// y a t il besoin de 2 timers pour la gestion des appuis longs ?
	// on ne peut pas se debrouiller en divisant la clock ?
}

void INT_init(void)
{
	__builtin_disable_interrupts();
	INTCONbits.MVEC = 1;

	// HT16
	HT16_INT_POLARITY = FALLING_EDGE;
	HT16_INT_FLAG_CLR;
	HT16_INT_PRIORITY = 2;
	HT16_INT_ENABLE = INT_ENABLED;

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

	//I2C1
	I2C1_INT_FLAG_CLR;
	I2C1_INT_PRIORITY = 4;
	I2C1_INT_ENABLE = 1;

	//SPI1
	SPI1_INT_FLAGS_CLR_RX;
	SPI1_INT_FLAGS_CLR_TX;
	SPI1_INT_PRIORITIES = 4;
        //PERSISTANT !!!!!!!!!!!!!!!!!!!!!!!!!!!
	SPI1_RECEIVE_ENABLE = INT_DISABLED;
	SPI1_TRANSFER_ENABLE = INT_DISABLED;

         // MCP encoders interrupts
//         COD_MCP_INT_POLARITY = FALLING_EDGE;
//         COD_MCP_INT_FLAG = 0; // Reset the flag
//         IPC1bits.INT1IP = 3; // Set priority
//         IEC0bits.INT1IE = 1; // Enable interrupt
	
         INTCONbits.INT3EP = FALLING_EDGE;
         IFS0bits.INT3IF = 0; // Reset the flag
         IPC3bits.INT3IP = 2; // Set priority
         IEC0bits.INT3IE = 1; // Enable interrupt

	__builtin_enable_interrupts();
}

void SPI1_init(void)
{
	SPI1CON = 0;
	SPI1BUF = 0;
	SPI1BRG = 0; //set baudrate 1Mhz suivant 8 Mhz du pbclk
	SPI1CONbits.CKE = 1;
	SPI1CONbits.CKP = 0; // mode 00 tás vue
	SPI1CONbits.MODE16 = 0;
	SPI1CONbits.MODE32 = 0;
	SPI1CONbits.MSTEN = 1; //activer master mode
	SPI1CONbits.ON = 1; //ON SPI1
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
	I2C1BRG = (FREQUENCY / ( 2 * 400000)) - 2 ;
	I2C1CONbits.DISSLW = 0;
        I2C1CONbits.SMEN = 1;
	I2C1CONbits.STREN = 1;
	I2C1CONbits.RCEN = 1;
	I2C1CONbits.ON = 1;
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
		I2C1_write(0xE0, config[i], NULL, 0);
		while (!(I2C1_state == E_I2C1_DONE))
			WDTCONbits.WDTCLR = 1;
		i++;
	}

	I2C1_write(0xE0, 0x08, message, 8);
	while (!(I2C1_state == E_I2C1_DONE))
		WDTCONbits.WDTCLR = 1; // CLEAR WATCHDOG

	I2C1_read_callback(0xE0, 0x40, 6, NULL);
	while (!(I2C1_state == E_I2C1_DONE))
		WDTCONbits.WDTCLR = 1; // CLEAR WATCHDOG
}

void MCP_LCD_init(void)
{
    SPI1CONbits.MODE16 = 1;

        //pins en output
        CS_MCP_LCD = 0x0;
        SPI1BUF = 0x4000;
        while (SPI1STATbits.SPIBUSY) ;
        SPI1BUF = 0x0000;
        while (SPI1STATbits.SPIBUSY) ;
        CS_MCP_LCD = 0x1;


        //mode sequentiel off
        CS_MCP_LCD = 0x0;
        SPI1BUF = 0x400A;
        while (SPI1STATbits.SPIBUSY) ;
        SPI1BUF = 0x2020;
        while (SPI1STATbits.SPIBUSY) ;
        CS_MCP_LCD = 0x1;

        SPI1CONbits.MODE16 = 0;
}

void MCP_ENCODERS_init_blocking(void)
{

    u16 read;

    SPI1CONbits.MODE16 = 1;

        CS_MCP_ENCODERS = 0x1;

        //read GPIO pour clear flags
        CS_MCP_ENCODERS = 0x0;
        SPI1BUF = 0x4110;
        while (SPI1STATbits.SPIBUSY) ;
        read = SPI1BUF;
        SPI1BUF = 0x0000;
        while (SPI1STATbits.SPIBUSY) ;
        read = SPI1BUF;
        CS_MCP_ENCODERS = 0x1;


        //activation des interrupts sur le port A
        CS_MCP_ENCODERS = 0x0;
        SPI1BUF = 0x4004;
        while (SPI1STATbits.SPIBUSY) ;
        read = SPI1BUF;
        SPI1BUF = 0xFFFF;
        while (SPI1STATbits.SPIBUSY) ;
        read = SPI1BUF;
        CS_MCP_ENCODERS = 0x1;

        SPI1CONbits.MODE16 = 0;
}

void    LCD_init(void)
{
        u8  line = 0;
        u8  col;

        // clear du LCD
        CS_MCP_LCD = 0x0;
        SPI1BUF = 0x4012;
        while (SPI1STATbits.SPIBUSY)
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
        CS_MCP_LCD = 0x1;
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
