#ifndef OX2A002A_H
#define	OX2A002A_H

#include <stdint.h>

#define LED_ON_OFF LATFbits.LATF1
#define BUTTON_SWITCH_OPEN PORTDbits.RD8
#define F1_IO_SELECTOR TRISFbits.TRISF1
#define F5_IO_SELECTOR TRISFbits.TRISF5
#define D8_IO_SELECTOR TRISDbits.TRISD8
#define FREQUENCY (8000000ul)
#define BUTTON_POLL_DELAY_MS 50
#define LONG_PRESS_LIMIT 100

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

#define MAX_WRITE_BUF 10
#define READ_BUF_SIZE 10

typedef signed char s8;
typedef signed short s16;
typedef signed long s32;
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;

enum E_I2C2_READ_REQUEST
{
    E_NONE = 0,
    E_KEYSCAN
};

enum E_EVENT_TYPE
{
    E_EVENT_NONE = 0,
    E_KEY_PRESSED,
    E_KEY_RELEASED,
    E_KEY_LONG_PRESSED
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
    E_I2C2_STOP,
    E_I2C2_WRITING_SLAVE_ADDR_WRITE,
    E_I2C2_WRITING_COMMAND_REGISTER,
    E_I2C2_SENDING_CMD_REGISTER_STOP,
    E_I2C2_SENDING_CMD_REGISTER_RESTART,
    E_I2C2_WRITING_SLAVE_ADDR_READ,
    E_I2C2_START_READING_BYTE,
    E_I2C2_READ_AND_ACK,
    E_I2C2_READ_AND_NACK,
    E_I2C2_READ_RESULT

};

void    push_I2C2_fifo(u8 mode, u8 data);
void 	I2C2_push(u8 data);
void 	I2C2_write(void);


#endif	/* OX2A002A_H */
