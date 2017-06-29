#ifndef OX2A002A_H
#define	OX2A002A_H

//#include <stdint.h>


#define FREQUENCY (8000000ul)
#define BUTTON_POLL_DELAY_MS	50
#define LONG_PRESS_LIMIT	100

// CONSTANTS
#define GPIO_INPUT	1
#define GPIO_OUTPUT	0
#define RISING_EDGE	1
#define FALLING_EDGE	0
#define FLAG_UP	1
#define FLAG_DOWN	0
#define INT_ENABLED	1
#define INT_DISABLED	0

//MAIN ENCODER
#define COD_A_GPIO	TRISBbits.TRISB2
#define COD_A_INT_POLARITY	INTCONbits.INT2EP
#define COD_A_INT_FLAG	IFS0bits.INT2IF
#define COD_A_INT_FLAG_CLR IFS0CLR = 11
#define COD_A_INT_PRIORITY	IPC2bits.INT2IP
#define COD_A_INT_ENABLE	IEC0bits.INT2IE
#define COD_B_GPIO	TRISBbits.TRISB3
#define COD_B_INT_POLARITY	INTCONbits.INT4EP
#define COD_B_INT_FLAG_CLR IFS0CLR = 19
#define COD_B_INT_FLAG	IFS0bits.INT4IF
#define COD_B_INT_PRIORITY	IPC4bits.INT4IP
#define COD_B_INT_ENABLE	IEC0bits.INT4IE

//TIMERS
// timer 2-3 used for bpm management
#define TIMER3_STOP_AND_RESET T2CON = 0
#define TIMER3_32_BITS_MODE T2CONbits.T32
#define TIMER3_VALUE TMR2
#define TIMER3_INT_FLAG_CLR IFS0CLR = 12
#define TIMER3_INT_PRIORITY IPC3bits.T3IP
#define TIMER3_INT_ENABLE IEC0bits.T3IE
// timer 4-5 used for key press management
#define TIMER5_STOP_AND_RESET T4CON = 0
#define TIMER5_32_BITS_MODE T4CONbits.T32
#define TIMER5_VALUE T5CON = 0
#define TIMER5_PERIOD PR4
#define TIMER5_INT_FLAG_CLR IFS0CLR = 20
#define TIMER5_INT_PRIORITY IPC5bits.T5IP
#define TIMER5_INT_ENABLE IEC0bits.T5IE


#define SS_MCP_LCD                LATGbits.LATG9
#define SS_MCP_ENCODERS           LATGbits.LATG9
#define SS_FLASH                LATGbits.LATG9

// GPIO MCP =
// GPA                                  GPB
// 7 6 5 4      3   2   1   0           7   to 0
// x x x CS2    CS1 E   RW  DIouRS      DB7 to DB0

#define LCD_DI_BIT      0x01
#define LCD_RW_BIT      0x02
#define LCD_ENABLE_BIT 0x04
#define LCD_CS1_bit 0x08
#define LCD_CS2_bit 0x10
#define LCD_PORTS_ADDRESS 0x4012

#define MCP_ENC_READ_INT_FLAG 0x410E0000
#define MCP_ENC_READ_INT_CAP  0x41100000

#define PBCLK 8000000
#define MIDI_BAUD_RATE 31250
#define SET_MIDI_BRG ((u16)(PBCLK / (16 * MIDI_BAUD_RATE) - 1))

#define notestatus  0
#define notevalue   1
#define notevelo    2

#define I2C2_READ   0
#define I2C2_WRITE  1

#define I2C2_ACK    0
#define I2C2_NACK   1

#define I2C2_READY (I2C2_state == E_I2C2_DONE)
#define SPI2_READY (SPI2_state == E_SPI2_DONE)

#define MAX_WRITE_BUF 10
#define READ_BUF_SIZE 10

#define SD_R1 1
#define SD_R3 3
#define SD_R7 7
#define SD_RETRIES 24
#define SD_BLOCK_SIZE   512

#define SD_SDSC 0
#define SD_SDHC 1

#define SD_WRITE_NO_ERROR 1
#define SD_WRITE_ERROR_WRONG_R1 2
#define SD_WRITE_ERROR_WRONG_DATA_RESPONSE 3
#define SD_WRITE_ERROR_WRONG_CRC 5
#define SD_WRITE_ERROR_WRONG_CRC 6

#define SD_READ_NO_ERROR 1
#define SD_READ_ERROR_WRONG_R1 2
#define SD_WRITE_ERROR_WRONG_TOKEN 3

typedef signed char s8;
typedef signed short s16;
typedef signed long s32;
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;

typedef void (* read_callback)(u8 *);
typedef void (* write_callback)(void);

enum E_EVENT_SOURCE
{
E_SOURCE_ENCODER_MAIN = 8
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

enum E_I2C2_STATE {
E_I2C2_DONE = 0,
E_I2C2_WRITE,
E_I2C2_WRITING_ADDR_CMD,
E_I2C2_RESTART_ADDR_CMD,
E_I2C2_SEND_SLAVE_ADDR,
E_I2C2_READ,
E_I2C2_NACK,
E_I2C2_ACK,
E_I2C2_WRITING_SLAVE_ADDR_WRITE,
E_I2C2_WRITING_COMMAND_REGISTER,
E_I2C2_SENDING_CMD_REGISTER_STOP,
E_I2C2_SENDING_CMD_REGISTER_RESTART,
E_I2C2_WRITING_SLAVE_ADDR_READ,
E_I2C2_START_READING_BYTE,
E_I2C2_READ_AND_ACK,
E_I2C2_READ_AND_NACK,
E_I2C2_CALLBACK
};

enum E_SPI2_STATE {
E_SPI2_DONE = 0,
E_SPI2_LCD_CONFIG,
E_SPI2_LCD_WRITE_ENABLE_HIGH,
E_SPI2_LCD_WRITE_ENABLE_LOW,
E_SPI2_LCD_RELEASE,
E_SPI2_ENC_READ_INT_FLAG,
E_SPI2_ENC_READ_INT_CAP,
E_SPI2_ENC_RELEASE
};

enum E_SPI2_SLAVE {
E_SPI2_SS_MCP_LCD,
E_SPI2_SS_MCP_ENC,
E_SPI2_SS_SD,
E_SPI2_SS_FLASH
};



//#define FLASH_READ              0x03
//#define FLASH_4K_ERASE          0x20
//#define FLASH_BYTE_PROGRAM      0x02
//#define FLASH_AAI_WORD          0xAD
//#define FLASH_RDSR              0x05
//#define FLASH_EWSR              0x50
//#define FLASH_WRSR              0x01
//#define FLASH_WREN              0x06

#endif	/* OX2A002A_H */
