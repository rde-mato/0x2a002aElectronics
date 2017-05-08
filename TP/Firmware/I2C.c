#include <xc.h>
#include "tp.h"
#include <sys/attribs.h>


u8 I2C2_state = E_I2C2_DONE;
u8 I2C2_RW;

u8 I2C2_write_buf[MAX_WRITE_BUF] = { 0 };
u8 I2C2_write_buf_index = 0;
u8 I2C2_write_buf_size = 0;

u8 I2C2_read_buf[READ_BUF_SIZE];
u8 I2C2_read_buf_expected;
u8 I2C2_read_buf_index = 0;

u8 read_buf_dirty;
extern u8   ks_dirty;

extern u32 buttonmatrix[16];

void __ISR(_I2C_2_VECTOR, IPL4AUTO) I2C2Handler(void)
{
    if (I2C2_RW == I2C2_WRITE)
        I2C2_state_machine_write();
    else
        I2C2_state_machine_read();
    IFS1bits.I2C2MIF = 0; // Reset the flag
}

void I2C2_push(u8 data)
{
    I2C2_write_buf[I2C2_write_buf_size++] = data;
}

void I2C2_write(void)
{
    I2C2_RW = I2C2_WRITE;
    I2C2_state = E_I2C2_WRITE;
    I2C2CONbits.SEN = 1;
}

void I2C2_state_machine_write(void)
{
    switch (I2C2_state)
    {
        case E_I2C2_WRITE:
            if (I2C2STATbits.TRSTAT)
                break;
            if (I2C2_write_buf_index < I2C2_write_buf_size)
                    I2C2TRN = I2C2_write_buf[I2C2_write_buf_index++];
            else
            {
                I2C2CONbits.PEN = 1;
                I2C2_state = E_I2C2_STOP;
            }
           break;
        case E_I2C2_STOP:
            I2C2_write_buf_index = 0;// A PRIORI PAS BESOIN DE I2C2STATbits.P
            I2C2_write_buf_size = 0;
            I2C2_state = E_I2C2_DONE;
            break;
    }
}


void I2C2_read(u8 expected_bytes)
{
    I2C2_RW = I2C2_READ;
    I2C2_state = E_I2C2_WRITING_SLAVE_ADDR_WRITE;
    I2C2_read_buf_expected = expected_bytes;
    I2C2CONbits.SEN = 1;
}

void I2C2_state_machine_read(void)
{
    switch (I2C2_state)
    {
        case E_I2C2_WRITING_SLAVE_ADDR_WRITE:
            I2C2TRN = I2C2_write_buf[0];
            I2C2_state = E_I2C2_WRITING_COMMAND_REGISTER;
            break;
        case E_I2C2_WRITING_COMMAND_REGISTER:
            I2C2TRN = I2C2_write_buf[1];
            I2C2_state = E_I2C2_SENDING_CMD_REGISTER_RESTART;
            break;
        case E_I2C2_SENDING_CMD_REGISTER_RESTART:
            I2C2CONbits.RSEN = 1;
            I2C2_state = E_I2C2_WRITING_SLAVE_ADDR_READ;
            break;
        case E_I2C2_WRITING_SLAVE_ADDR_READ:
            I2C2TRN = I2C2_write_buf[0] | 1;
            I2C2_state = E_I2C2_START_READING_BYTE;
            break;
        case E_I2C2_READ_AND_ACK:
            if (I2C2STATbits.RBF)
            {
                I2C2_read_buf[I2C2_read_buf_index++] = I2C2RCV;
                I2C2CONbits.ACKDT = 0;
                I2C2CONbits.ACKEN = 1;
                I2C2_state = E_I2C2_START_READING_BYTE;
            }
        case E_I2C2_START_READING_BYTE:
            if (I2C2_read_buf_index < I2C2_read_buf_expected - 1)
            {
                I2C2_state = E_I2C2_READ_AND_ACK;
                I2C2CONbits.RCEN = 1;
            }
            else
            {
                I2C2_state = E_I2C2_READ_AND_NACK;
                I2C2CONbits.RCEN = 1;
            }
            break;
        case E_I2C2_READ_AND_NACK:
            if (I2C2STATbits.RBF)
            {
                I2C2_read_buf[I2C2_read_buf_index++] = I2C2RCV;
                I2C2CONbits.ACKDT = 1;
                I2C2CONbits.ACKEN = 1;
                I2C2CONbits.PEN = 1;
                I2C2_write_buf_index = 0;
                I2C2_write_buf_size = 0;
                I2C2_read_buf_index = 0;
                I2C2_read_buf_expected = 0; // PAS SUR
                I2C2_state = E_I2C2_DONE;
            }
            break;
    }
}

