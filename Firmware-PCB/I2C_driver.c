#include <xc.h>
#include <sys/attribs.h>
#include "0x2a002a.h"

extern u8               HT16_write_leds_request;
extern u8               HT16_read_keys_request;

u8                      I2C1_read_buf[I2C1_READ_BUF_SIZE];

u8               I2C1_state = E_I2C1_DONE;
u8               I2C1_RW;
u8               I2C1_write_buf[I2C1_WRITE_BUF_SIZE] = { 0 };
u8               I2C1_write_buf_index = 0;
u8               I2C1_write_buf_size = 0;
u8               I2C1_read_buf_expected;
u8               I2C1_read_buf_index = 0;
read_callback    I2C1_read_cb = NULL;
write_callback   I2C1_write_cb = NULL;

void    int_init_I2C1(void)
{
    I2C1_INT_PRIORITY = 4;
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
    I2C1CONbits.DISSLW = 1;
    I2C1CONbits.SMEN = 0;
//    I2C1CONbits.STREN = 1; Inutile ? slave mode only
    I2C1CONbits.RCEN = 1;
    I2C1CONbits.ON = 1;
}


void I2C1_push(u8 data)
{
    I2C1_write_buf[I2C1_write_buf_size++] = data;
}

void I2C1_write(u8 slave, u8 command, u8 *buffer, u32 size)
{
    u32	i;

    I2C1_push(slave);
    I2C1_push(command);
    if (buffer && size)
    {
        i = 0;
        while (i < size)
            I2C1_push(buffer[i++]);
    }
    I2C1_write_cb = NULL;
    I2C1_RW = I2C1_RW_WRITE;
    I2C1_state = E_I2C1_WRITE;
    I2C1CONbits.SEN = 1;
    I2C1_INT_ENABLE = INT_ENABLED;
}

void I2C1_write_callback(u8 slave, u8 command, u8 *buffer, u32 size, write_callback cb)
{
    u32	i;

    I2C1_push(slave);
    I2C1_push(command);
    if (buffer)
    {
        i = 0;
        while (i < size)
            I2C1_push(buffer[i++]);
    }
    I2C1_write_cb = cb;
    I2C1_RW = I2C1_RW_WRITE;
    I2C1_state = E_I2C1_WRITE;
    I2C1CONbits.SEN = 1;
    I2C1_INT_ENABLE = INT_ENABLED;
}

void I2C1_read_callback(u8 slave, u8 command, u8 size, read_callback cb)
{
    I2C1_push(slave);
    I2C1_push(command);
    I2C1_read_cb = cb;
    I2C1_RW = I2C1_RW_READ;
    I2C1_state = E_I2C1_WRITING_SLAVE_ADDR_WRITE;
    I2C1_read_buf_expected = size;
    I2C1CONbits.SEN = 1;
    I2C1_INT_ENABLE = INT_ENABLED;
}

void HT16_init(void)
{
    u8  message[8] = {0};
    u8	i;

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

void I2C1_state_machine_write(void)
{
    switch (I2C1_state)
    {
        case E_I2C1_WRITE:
//            if (I2C1STAT != 8 || I2C1STAT != 0)
//                break;
//            if (!I2C1STATbits.TRSTAT)
//            {
                if (I2C1_write_buf_index < I2C1_write_buf_size)
                {
                    I2C1TRN = I2C1_write_buf[I2C1_write_buf_index++];
                    I2C1_INT_ENABLE = INT_ENABLED;
                }
                else
                {
                    I2C1_state = E_I2C1_CALLBACK;
                    I2C1CONbits.PEN = 1;
                    I2C1_INT_ENABLE = INT_ENABLED;
                }
//            }
            break;
        case E_I2C1_CALLBACK:
            if (I2C1_write_cb)
                (*I2C1_write_cb)();
            I2C1_write_buf_index = 0;
            I2C1_write_buf_size = 0;
            I2C1_write_cb = NULL;
            I2C1_state = E_I2C1_DONE;
        break;
    }
}

void I2C1_state_machine_read(void)
{
    switch (I2C1_state)
    {
        case E_I2C1_WRITING_SLAVE_ADDR_WRITE:
            I2C1TRN = I2C1_write_buf[0];
            I2C1_state = E_I2C1_WRITING_COMMAND_REGISTER;
            I2C1_INT_ENABLE = INT_ENABLED;
            break;
        case E_I2C1_WRITING_COMMAND_REGISTER:
            I2C1TRN = I2C1_write_buf[1];
            I2C1_state = E_I2C1_SENDING_CMD_REGISTER_RESTART;
            I2C1_INT_ENABLE = INT_ENABLED;
            break;
        case E_I2C1_SENDING_CMD_REGISTER_RESTART:
            I2C1CONbits.RSEN = 1;
            I2C1_state = E_I2C1_WRITING_SLAVE_ADDR_READ;
            I2C1_INT_ENABLE = INT_ENABLED;
            break;
        case E_I2C1_WRITING_SLAVE_ADDR_READ:
            I2C1TRN = I2C1_write_buf[0] | 1;
            I2C1_state = E_I2C1_START_READING_BYTE;
            I2C1_INT_ENABLE = INT_ENABLED;
            break;
        case E_I2C1_READ_AND_ACK:
            if (I2C1STATbits.RBF)
            {
                I2C1_read_buf[I2C1_read_buf_index++] = I2C1RCV;
                I2C1CONbits.ACKDT = 0;
                I2C1CONbits.ACKEN = 1;
                I2C1_state = E_I2C1_START_READING_BYTE;
                I2C1_INT_ENABLE = INT_ENABLED;
            }
            break;
        case E_I2C1_START_READING_BYTE:
            if (I2C1_read_buf_index < I2C1_read_buf_expected - 1)
            {
                I2C1_state = E_I2C1_READ_AND_ACK;
                I2C1CONbits.RCEN = 1;
                I2C1_INT_ENABLE = INT_ENABLED;
            }
            else
            {
                I2C1_state = E_I2C1_READ_AND_NACK;
                I2C1CONbits.RCEN = 1;
                I2C1_INT_ENABLE = INT_ENABLED;
            }
            break;
        case E_I2C1_READ_AND_NACK:
            if (I2C1STATbits.RBF)
            {
                I2C1_read_buf[I2C1_read_buf_index] = I2C1RCV;
                I2C1CONbits.ACKDT = 1;
                I2C1CONbits.ACKEN = 1; // a noter que ceci declenchera une interrupt.
                I2C1CONbits.PEN = 1;
                I2C1_write_buf_index = 0;
                I2C1_write_buf_size = 0;
                I2C1_read_buf_index = 0;
                I2C1_read_buf_expected = 0; // PAS SUR
                I2C1_state = E_I2C1_CALLBACK;
                I2C1_INT_ENABLE = INT_ENABLED;
            }
            break;
        case E_I2C1_CALLBACK:
            if (I2C1_read_cb)
                (*I2C1_read_cb)(I2C1_read_buf);
            I2C1_read_cb = NULL;
            I2C1_state = E_I2C1_DONE;
            break;
    }
}


void __ISR(_I2C_1_VECTOR, IPL4AUTO) I2C1Handler(void)
{
    I2C1_INT_ENABLE = INT_DISABLED;
    if (I2C1_RW == I2C1_RW_WRITE)
        I2C1_state_machine_write();
    else
        I2C1_state_machine_read();
}

void    I2C1_manager(void)
{
    if (!I2C1_READY)
        return ;
    if (HT16_write_leds_request)
    {
        led_refresh();
    }
    else if (HT16_read_keys_request)
    {
        HT16_read_keys_request = 0;
        key_scan();
    }
}