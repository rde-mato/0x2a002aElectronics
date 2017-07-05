#ifndef OX2A002A_H
#define	OX2A002A_H

//#include <stdint.h> //


#define FREQUENCY				(8000000ul)
#define ONE_MILLISECOND         		(8000ul)
#define BUTTON_POLL_DELAY_MS	50
#define LONG_PRESS_LIMIT		100
#define SCREEN_DURATION_MS		1000

#define INSTRUMENTS_COUNT       16
#define PATTERNS_PER_INSTRUMENT 16
#define QTIME_PER_INSTRUMENT    16
#define NOTES_PER_QTIME         4
#define ATTRIBUTES_PER_NOTE     3

// CONSTANTS
#define DEFAULT_BPM             142
#define GPIO_INPUT		1
#define GPIO_OUTPUT		0
#define RISING_EDGE		1
#define FALLING_EDGE	0
#define FLAG_UP			1
#define FLAG_DOWN		0
#define INT_ENABLED		1
#define INT_DISABLED	0
#define CS_LINE_UP		1
#define CS_LINE_DOWN	0
#define DIGITAL_PIN		0
#define ANALOG_PIN		1

//MAIN ENCODER
#define COD_A_GPIO			TRISBbits.TRISB2
#define COD_A_INT_POLARITY	INTCONbits.INT2EP
#define COD_A_INT_FLAG		IFS0bits.INT2IF
#define COD_A_INT_FLAG_CLR	IFS0CLR = (1 << 13)
#define COD_A_INT_PRIORITY	IPC2bits.INT2IP
#define COD_A_INT_ENABLE	IEC0bits.INT2IE
#define COD_B_GPIO			TRISBbits.TRISB3
#define COD_B_INT_POLARITY	INTCONbits.INT4EP
#define COD_B_INT_FLAG_CLR	IFS0CLR = (1 << 23)
#define COD_B_INT_FLAG		IFS0bits.INT4IF
#define COD_B_INT_PRIORITY	IPC4bits.INT4IP
#define COD_B_INT_ENABLE	IEC0bits.INT4IE

//MCP ENCODERS
#define COD_MCP_A_GPIO			TRISBbits.TRISB0
#define COD_MCP_B_GPIO			TRISBbits.TRISB1
#define COD_MCP_INT_POLARITY	INTCONbits.INT1EP
#define COD_MCP_INT_FLAG		IFS0bits.INT1IF

//PERIPHERAL PIN SELECT
#define PPS_COD_A			INT2R = 0b0100
#define COD_A_ANALOG		ANSELBbits.ANSB2
#define PPS_COD_B			INT4R = 0b0001
#define COD_B_ANALOG		ANSELBbits.ANSB3
#define PPS_SDI1			SDI1R = 0b0000
#define SDI1_ANALOG			ANSELAbits.ANSA1
#define PPS_SDO1			RPB13R = 0b0011
#define SDO1_ANALOG			ANSELBbits.ANSB13
#define SPI1_CS2_ANALOG		ANSELBbits.ANSB12
#define SPI1_CS3_ANALOG		ANSELBbits.ANSB15
#define PPS_MIDI			RPA0R = 0b0001
#define MIDI_ANALOG			ANSELAbits.ANSA0
#define PPS_MCP_ENC_A		INT1R = 0b0010
#define MCP_ENC_A_ANALOG	ANSELBbits.ANSB0
#define PPS_MCP_ENC_B		INT3R = 0b0010
#define MCP_ENC_B_ANALOG	ANSELBbits.ANSB1

//TIMERS
#define TIMER_PRESCALE_1        0b000
#define TIMER_PRESCALE_2        0b001
#define TIMER_PRESCALE_4        0b010
#define TIMER_PRESCALE_8        0b011
#define TIMER_PRESCALE_16       0b100
#define TIMER_PRESCALE_32       0b101
#define TIMER_PRESCALE_64       0b110
#define TIMER_PRESCALE_256	0b111

// timer 2 used for pattern management
#define TIMER2_STOP_AND_RESET	T2CON = 0
#define TIMER2_VALUE		TMR2
#define TIMER2_PERIOD           PR2
#define TIMER2_PRESCALE         T2CONbits.TCKPS
#define TIMER2_INT_FLAG_CLR     IFS0CLR = (1 << 9)
#define TIMER2_INT_PRIORITY     IPC2bits.T2IP
#define TIMER2_INT_ENABLE	IEC0bits.T2IE

// timer 3 used for bpm button
#define TIMER3_STOP_AND_RESET   T3CON = 0
#define TIMER3_VALUE            TMR3
#define TIMER3_PERIOD           PR3
#define TIMER3_PRESCALE         T3CONbits.TCKPS
#define TIMER3_INT_FLAG_CLR     IFS0CLR = (1 << 14)
#define TIMER3_INT_PRIORITY     IPC3bits.T3IP
#define TIMER3_INT_ENABLE       IEC0bits.T3IE

// timer 4 used for duration of templates
#define TIMER4_STOP_AND_RESET	T4CON = 0
#define TIMER4_VALUE			TMR4
#define TIMER4_PERIOD			PR4
#define TIMER4_PRESCALE			T4CONbits.TCKPS
#define TIMER4_INT_FLAG_CLR		IFS0CLR = (1 << 19)
#define TIMER4_INT_PRIORITY		IPC4bits.T4IP
#define TIMER4_INT_ENABLE		IEC0bits.T4IE

// timer 5 used for key press management
#define TIMER5_STOP_AND_RESET	T5CON = 0
#define TIMER5_VALUE			TMR5
#define TIMER5_PERIOD			PR5
#define TIMER5_PRESCALE			T5CONbits.TCKPS
#define TIMER5_INT_FLAG_CLR		IFS0CLR = (1 << 24)
#define TIMER5_INT_PRIORITY		IPC5bits.T5IP
#define TIMER5_INT_ENABLE		IEC0bits.T5IE

//I2C1
#define I2C1_PIN_GPIO			TRISBbits.TRISB8
#define I2C1_PIN_LATCH 			LATBbits.LATB8
#define I2C1_INT_FLAG_CLR		IFS1CLR = (1 << 12)
#define I2C1_INT_PRIORITY		IPC8bits.I2C1IP
#define I2C1_INT_ENABLE			IEC1bits.I2C1MIE
#define I2C1_READ				0
#define I2C1_WRITE				1
#define I2C1_ACK				0
#define I2C1_NACK				1
#define I2C1_READY				(I2C1_state == E_I2C1_DONE)

//HT16
#define HT16_INT_POLARITY	INTCONbits.INT0EP
#define HT16_INT_FLAG		IFS0bits.INT0IF
#define HT16_INT_FLAG_CLR	IFS0CLR = (1 << 3)
#define HT16_INT_PRIORITY	IPC0bits.INT0IP
#define HT16_INT_ENABLE		IEC0bits.INT0IE

//SPI1
#define SPI1_CS0_GPIO				TRISBbits.TRISB10
#define SPI1_CS1_GPIO				TRISBbits.TRISB11
#define SPI1_CS2_GPIO				TRISBbits.TRISB12
#define SPI1_CS3_GPIO				TRISBbits.TRISB15
#define CS_MCP_LCD					LATBbits.LATB10
#define CS_MCP_ENCODERS				LATBbits.LATB11
#define CS_EEPROM					LATBbits.LATB12
#define CS_SD						LATBbits.LATB15
#define SPI1_RECEIVE_FLAG			IFS1bits.SPI1RX
#define SPI1_TRANSFER_FLAG			IFS1bits.SPI1TX
#define SPI1_INT_FLAGS_CLR_RX		IFS1CLR = (1 << 5)
#define SPI1_INT_FLAGS_CLR_TX		IFS1CLR = (1 << 6)
#define SPI1_INT_PRIORITIES			IPC7bits.SPI1IP
#define SPI1_RECEIVE_ENABLE			IEC1bits.SPI1RXIE
#define SPI1_TRANSFER_ENABLE		IEC1bits.SPI1TXIE

//UART

#define UART1_TX_INT_ENABLE     IEC1bits.U1TXIE
#define UART1_TX_INT_PRIORITY   IPC8bits.U1IP
#define UART1_TX_INT_FLAG       IFS1bits.U1TXIF
#define UART1_TX_INT_FLAG_CLR   IFS1CLR = (1 << 8)

// GPIO MCP =
// GPA                                  GPB
// 7  6  5  4    3    2  1   0           7   to 0
// x  x  x  CS2  CS1  E  RW  DIouRS      DB7 to DB0

#define LCD_DI_BIT          0x01
#define LCD_RW_BIT          0x02
#define LCD_ENABLE_BIT      0x04
#define LCD_CS1_bit         0x08
#define LCD_CS2_bit         0x10
#define LCD_PORTS_ADDRESS   0x4012
#define LINE_MAX_LEN        22

#define MCP_ENC_READ_INT_FLAG 0x410E0000
#define MCP_ENC_READ_INT_CAP  0x41100000

#define PBCLK 8000000
#define MIDI_BAUD_RATE 31250
#define SET_MIDI_BRG ((u16)(PBCLK / (16 * MIDI_BAUD_RATE) - 1))

#define notestatus  0
#define notevalue   1
#define notevelo    2

#define SPI1_READY (SPI1_state == E_SPI1_DONE)

#define MAX_WRITE_BUF 10
#define READ_BUF_SIZE 10

#define SD_RETRIES 1000
#define SD_BLOCK_SIZE   512


typedef signed char s8;
typedef signed short s16;
typedef signed long s32;
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;

typedef void (* read_callback)(u8 *);
typedef void (* write_callback)(void);

enum E_EVENT_SOURCE // dans l'ordre de mapping des boutons, ne pas changer
{
	E_SOURCE_KEY_0,
	E_SOURCE_KEY_1,
	E_SOURCE_KEY_2,
	E_SOURCE_KEY_3,
	E_SOURCE_KEY_4,
	E_SOURCE_KEY_5,
	E_SOURCE_KEY_6,
	E_SOURCE_KEY_7,
	E_SOURCE_KEY_8,
	E_SOURCE_KEY_9,
	E_SOURCE_KEY_10,
	E_SOURCE_KEY_11,
	E_SOURCE_KEY_12,
	E_SOURCE_KEY_13,
	E_SOURCE_KEY_14,
	E_SOURCE_KEY_15,
	E_SOURCE_BUTTON_PLAY_PAUSE,
	E_SOURCE_BUTTON_CUE,
	E_SOURCE_BUTTON_REC,
	E_SOURCE_BUTTON_TAP,

	E_SOURCE_BUTTON_EDIT,
	E_SOURCE_BUTTON_INSTRUMENT,
	E_SOURCE_BUTTON_PATTERN,
	E_SOURCE_BUTTON_KEYBOARD,



	E_SOURCE_ENCODER_0,
	E_SOURCE_ENCODER_1,
	E_SOURCE_ENCODER_2,
	E_SOURCE_ENCODER_3,
	E_SOURCE_ENCODER_4,
	E_SOURCE_ENCODER_5,
	E_SOURCE_ENCODER_6,
	E_SOURCE_ENCODER_7,
	E_SOURCE_ENCODER_MAIN
};

enum E_MODES
{
	E_MODE_DEFAULT,
        E_MODE_PATTERN,
	E_MODE_MENU,
        E_MODE_INSTRU,
        E_MODE_KEYBOARD
};

enum E_EVENT_TYPE
{
	E_EVENT_NONE = 0,
	E_KEY_PRESSED,
	E_KEY_RELEASED,
	E_KEY_LONG_PRESSED,
	E_ENCODER_TURNED_RIGHT,
	E_ENCODER_TURNED_LEFT
};

enum E_I2C1_STATE {
	E_I2C1_DONE = 0,
	E_I2C1_WRITE,
	E_I2C1_WRITING_ADDR_CMD,
	E_I2C1_RESTART_ADDR_CMD,
	E_I2C1_SEND_SLAVE_ADDR,
	E_I2C1_READ,
	E_I2C1_NACK,
	E_I2C1_ACK,
	E_I2C1_WRITING_SLAVE_ADDR_WRITE,
	E_I2C1_WRITING_COMMAND_REGISTER,
	E_I2C1_SENDING_CMD_REGISTER_STOP,
	E_I2C1_SENDING_CMD_REGISTER_RESTART,
	E_I2C1_WRITING_SLAVE_ADDR_READ,
	E_I2C1_START_READING_BYTE,
	E_I2C1_READ_AND_ACK,
	E_I2C1_READ_AND_NACK,
	E_I2C1_CALLBACK
};

enum E_SPI1_STATE {
	E_SPI1_DONE = 0,
	E_SPI1_LCD_CONFIG,
	E_SPI1_LCD_WRITE_ENABLE_HIGH,
	E_SPI1_LCD_WRITE_ENABLE_LOW,
	E_SPI1_LCD_RELEASE,
	E_SPI1_ENC_READ_INT_FLAG,
	E_SPI1_ENC_READ_INT_CAP,
	E_SPI1_ENC_RELEASE,
	E_SPI1_EEPROM_WRITE_ENABLE,
	E_SPI1_EEPROM_WRITE_ENABLE_OK,
	E_SPI1_EEPROM_WAIT_WRITE_STAT_OK,
	E_SPI1_EEPROM_STILL_WAIT_WRITE_STAT_OK,
	E_SPI1_EEPROM_WRITE_MODE,
	E_SPI1_EEPROM_WRITE_ADDRESS,
	E_SPI1_EEPROM_WRITE_DATA,
	E_SPI1_EEPROM_WRITE_DONE,
	E_SPI1_EEPROM_READ_ENABLE,
	E_SPI1_EEPROM_READ_ENABLE_OK,
	E_SPI1_EEPROM_READ_MODE,
	E_SPI1_EEPROM_READ_ADDRESS,
	E_SPI1_EEPROM_WAIT_READ_STAT_OK,
	E_SPI1_EEPROM_READ_DATA,
	E_SPI1_EEPROM_READ_DONE
};

enum E_SPI1_SLAVE {
	E_SPI1_CS_MCP_LCD = 0,
	E_SPI1_CS_MCP_ENC,
	E_SPI1_CS_SD,
	E_SPI1_CS_EEPROM
};

enum E_SD_CARD
{
	SD_R1,
	SD_R3,
	SD_R7,
	SD_SDSC,
	SD_SDHC,
	SD_WRITE_NO_ERROR,
	SD_WRITE_ERROR_WRONG_R1,
	SD_WRITE_ERROR_WRONG_DATA_RESPONSE,
	SD_WRITE_ERROR_WRONG_CRC,
	SD_WRITE_ERROR_WRONG_TOKEN,
	SD_READ_NO_ERROR,
	SD_READ_ERROR_WRONG_R1,
	SD_READ_ERROR_WRONG_TOKEN
};

u8	SPI1_state;
u8	SPI_eeprom_write_request;
u8	SPI_eeprom_read_request;
u16	eeprom_address;
u8	eeprom_buf_size;
u8	eeprom_buf[128]; //size of page



//#define FLASH_READ              0x03
//#define FLASH_4K_ERASE          0x20
//#define FLASH_BYTE_PROGRAM      0x02
//#define FLASH_AAI_WORD          0xAD
//#define FLASH_RDSR              0x05
//#define FLASH_EWSR              0x50
//#define FLASH_WRSR              0x01
//#define FLASH_WREN              0x06

#endif	/* OX2A002A_H */
