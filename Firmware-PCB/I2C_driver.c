#include <xc.h>
#include <sys/attribs.h>
#include "0x2a002a.h"


u8	I2C1_state = E_I2C1_DONE;

u8	I2C1_RW;

u8	I2C1_write_buf[MAX_WRITE_BUF] = { 0 };
u8	I2C1_write_buf_index = 0;
u8	I2C1_write_buf_size = 0;

u8	I2C1_read_buf[READ_BUF_SIZE];
u8	I2C1_read_buf_expected;
u8	I2C1_read_buf_index = 0;

read_callback I2C1_read_cb = NULL;
write_callback I2C1_write_cb = NULL;


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
	I2C1_RW = I2C1_WRITE;
	I2C1_state = E_I2C1_WRITE;
	I2C1CONbits.SEN = 1;
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
	I2C1_RW = I2C1_WRITE;
	I2C1_state = E_I2C1_WRITE;
	I2C1CONbits.SEN = 1;
}

void I2C1_state_machine_write(void)
{
	switch (I2C1_state)
	{
	  case E_I2C1_WRITE:
		  if (I2C1STATbits.TRSTAT)
			  break;
		  if (I2C1_write_buf_index < I2C1_write_buf_size)
			  I2C1TRN = I2C1_write_buf[I2C1_write_buf_index++];
		  else
		  {
		  I2C1_state = E_I2C1_CALLBACK;
		  I2C1CONbits.PEN = 1;
	  }
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

void I2C1_read_callback(u8 slave, u8 command, u8 size, read_callback cb)
{
	I2C1_push(slave);
	I2C1_push(command);
	I2C1_read_cb = cb;
	I2C1_RW = I2C1_READ;
	I2C1_state = E_I2C1_WRITING_SLAVE_ADDR_WRITE;
	I2C1_read_buf_expected = size;
	I2C1CONbits.SEN = 1;
}

void I2C1_state_machine_read(void)
{
	switch (I2C1_state)
	{
	  case E_I2C1_WRITING_SLAVE_ADDR_WRITE:
		  I2C1TRN = I2C1_write_buf[0];
		  I2C1_state = E_I2C1_WRITING_COMMAND_REGISTER;
		  break;
	  case E_I2C1_WRITING_COMMAND_REGISTER:
		  I2C1TRN = I2C1_write_buf[1];
		  I2C1_state = E_I2C1_SENDING_CMD_REGISTER_RESTART;
		  break;
	  case E_I2C1_SENDING_CMD_REGISTER_RESTART:
		  I2C1CONbits.RSEN = 1;
		  I2C1_state = E_I2C1_WRITING_SLAVE_ADDR_READ;
		  break;
	  case E_I2C1_WRITING_SLAVE_ADDR_READ:
		  I2C1TRN = I2C1_write_buf[0] | 1;
		  I2C1_state = E_I2C1_START_READING_BYTE;
		  break;
	  case E_I2C1_READ_AND_ACK:
		  if (I2C1STATbits.RBF)
		  {
		  I2C1_read_buf[I2C1_read_buf_index++] = I2C1RCV;
		  I2C1CONbits.ACKDT = 0;
		  I2C1CONbits.ACKEN = 1;
		  I2C1_state = E_I2C1_START_READING_BYTE;
	  }
		  break;
	  case E_I2C1_START_READING_BYTE:
		  if (I2C1_read_buf_index < I2C1_read_buf_expected - 1)
		  {
		  I2C1_state = E_I2C1_READ_AND_ACK;
		  I2C1CONbits.RCEN = 1;
	  }
		  else
		  {
		  I2C1_state = E_I2C1_READ_AND_NACK;
		  I2C1CONbits.RCEN = 1;
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
	I2C1_INT_FLAG_CLR;
	if (I2C1_RW == I2C1_WRITE)
		I2C1_state_machine_write();
	else
		I2C1_state_machine_read();
}
