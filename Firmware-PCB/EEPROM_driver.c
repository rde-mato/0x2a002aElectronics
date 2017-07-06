#include <xc.h>
#include <sys/attribs.h>
#include "0x2a002a.h"

extern u8   SPI1_state;

u8          SPI_eeprom_write_request;
u8          SPI_eeprom_read_request;

static u16  eeprom_address;
static u8   eeprom_buf_size;
static u8   eeprom_buf[128]; //size of page

void	eeprom_state_machine_write(void)
{
    u16		eeprom_read_byte;
    static u8	eeprom_buf_i = 0;

    switch (SPI1_state)
    {
        case E_SPI1_EEPROM_WRITE_ENABLE:
            eeprom_buf_i = 0;
            CS_EEPROM = 0;
            SPI1BUF = 0x06; // Write enable latch
            SPI1_state = E_SPI1_EEPROM_WRITE_ENABLE_OK;
            SPI1_RECEIVE_ENABLE = INT_ENABLED;
            SPI1_TRANSFER_ENABLE = INT_ENABLED;
            break;
        case E_SPI1_EEPROM_WRITE_ENABLE_OK:
            if (SPI1STATbits.SPIBUSY)
                break;
            eeprom_read_byte = SPI1BUF;
            CS_EEPROM = 1;
            SPI1_state = E_SPI1_EEPROM_WAIT_WRITE_STAT_OK;
            SPI1CONbits.MODE16 = 1;
            SPI1_RECEIVE_ENABLE = INT_ENABLED;
            SPI1_TRANSFER_ENABLE = INT_ENABLED;
            break;
        case E_SPI1_EEPROM_WAIT_WRITE_STAT_OK:
            CS_EEPROM = 0;
            SPI1BUF = 0x0500;// read status byte + blblbl
            SPI1_state = E_SPI1_EEPROM_STILL_WAIT_WRITE_STAT_OK;
            SPI1_RECEIVE_ENABLE = INT_ENABLED;
            SPI1_TRANSFER_ENABLE = INT_ENABLED;
            break;
        case E_SPI1_EEPROM_STILL_WAIT_WRITE_STAT_OK:
            eeprom_read_byte = SPI1BUF;
            if (eeprom_read_byte & 0x02) // Check in STATUS register if Write Enable Latch is ok
                SPI1_state = E_SPI1_EEPROM_WRITE_MODE;
            else
                SPI1_state = E_SPI1_EEPROM_WAIT_WRITE_STAT_OK;
            CS_EEPROM = 1;
            SPI1_RECEIVE_ENABLE = INT_ENABLED;
            SPI1_TRANSFER_ENABLE = INT_ENABLED;
            break;
        case E_SPI1_EEPROM_WRITE_MODE:
            if (SPI1STATbits.SPIBUSY)
                break;
            eeprom_read_byte = SPI1BUF;
            SPI1CONbits.MODE16 = 0;
            CS_EEPROM = 0;
            SPI1BUF = 0x02; // Write instruction
            SPI1_state = E_SPI1_EEPROM_WRITE_ADDRESS;
            SPI1_RECEIVE_ENABLE = INT_ENABLED;
            SPI1_TRANSFER_ENABLE = INT_ENABLED;
            break;
        case E_SPI1_EEPROM_WRITE_ADDRESS:
            if (SPI1STATbits.SPIBUSY)
                break;
            eeprom_read_byte = SPI1BUF;
            SPI1CONbits.MODE16 = 1;
            SPI1BUF = eeprom_address; // Send 16 bit address
            SPI1_state = E_SPI1_EEPROM_WRITE_DATA;
            SPI1_RECEIVE_ENABLE = INT_ENABLED;
            SPI1_TRANSFER_ENABLE = INT_ENABLED;
            break;
        case E_SPI1_EEPROM_WRITE_DATA:
            if (SPI1STATbits.SPIBUSY)
                break;
            eeprom_read_byte = SPI1BUF;
            SPI1CONbits.MODE16 = 0;
            if (eeprom_buf_i == eeprom_buf_size)
                SPI1_state = E_SPI1_EEPROM_WRITE_DONE;
            SPI1BUF = eeprom_buf[eeprom_buf_i++];
            SPI1_RECEIVE_ENABLE = INT_ENABLED;
            SPI1_TRANSFER_ENABLE = INT_ENABLED;
            break;
        case E_SPI1_EEPROM_WRITE_DONE:
            if (SPI1STATbits.SPIBUSY)
                break;
            eeprom_read_byte = SPI1BUF;
            CS_EEPROM = 1;
            SPI1_state = E_SPI1_DONE;
            eeprom_buf_i = 0;
            SPI_eeprom_write_request = 0;
            break;
    }
}


void    eeprom_state_machine_read(void)
{
    u16         eeprom_read_byte;
    static u8	eeprom_buf_i = 0;

    switch (SPI1_state)
    {
        case E_SPI1_EEPROM_READ_ENABLE:
            eeprom_buf_i = 0;
            SPI1CONbits.MODE16 = 1;
            CS_EEPROM = 0;
            SPI1BUF = 0x0500; // Read status byte instruction + blblbl
            SPI1_state = E_SPI1_EEPROM_READ_ENABLE_OK;
            SPI1_RECEIVE_ENABLE = INT_ENABLED;
            SPI1_TRANSFER_ENABLE = INT_ENABLED;
            break;
        case E_SPI1_EEPROM_READ_ENABLE_OK:
            eeprom_read_byte = SPI1BUF;
            if (eeprom_read_byte & 0x01) // Check in STATUS if Write In Progress is ON
                SPI1_state = E_SPI1_EEPROM_READ_ENABLE;
            else
                SPI1_state = E_SPI1_EEPROM_READ_MODE;
            CS_EEPROM = 1;
            SPI1_RECEIVE_ENABLE = INT_ENABLED;
            SPI1_TRANSFER_ENABLE = INT_ENABLED;
            break;
        case E_SPI1_EEPROM_READ_MODE:
            CS_EEPROM = 0;
            SPI1CONbits.MODE16 = 0;
            SPI1BUF = 0x03; //READ instruction
            SPI1_state = E_SPI1_EEPROM_READ_ADDRESS;
            SPI1_RECEIVE_ENABLE = INT_ENABLED;
            SPI1_TRANSFER_ENABLE = INT_ENABLED;
            break;
        case E_SPI1_EEPROM_READ_ADDRESS:
            if (SPI1STATbits.SPIBUSY)
                break;
            eeprom_read_byte = SPI1BUF;
            SPI1CONbits.MODE16 = 1;
            SPI1BUF = eeprom_address;
            SPI1_state = E_SPI1_EEPROM_WAIT_READ_STAT_OK;
            SPI1_RECEIVE_ENABLE = INT_ENABLED;
            SPI1_TRANSFER_ENABLE = INT_ENABLED;
            break;
        case E_SPI1_EEPROM_WAIT_READ_STAT_OK:
            if (SPI1STATbits.SPIBUSY)
                break;
            eeprom_read_byte = SPI1BUF;
            SPI1CONbits.MODE16 = 0;
            SPI1_state = E_SPI1_EEPROM_READ_DATA;
            SPI1BUF = 0x00;
            SPI1_RECEIVE_ENABLE = INT_ENABLED;
            SPI1_TRANSFER_ENABLE = INT_ENABLED;
            break;
        case E_SPI1_EEPROM_READ_DATA :
            if (SPI1STATbits.SPIBUSY)
                break;
            eeprom_buf[eeprom_buf_i++] = SPI1BUF;
            if (eeprom_buf_i == eeprom_buf_size)
                SPI1_state = E_SPI1_EEPROM_READ_DONE;
            else
                SPI1BUF = 0x00;
            SPI1_RECEIVE_ENABLE = INT_ENABLED;
            SPI1_TRANSFER_ENABLE = INT_ENABLED;
            break;
        case E_SPI1_EEPROM_READ_DONE:
            CS_EEPROM = 1;
            SPI1_state = E_SPI1_DONE;
            eeprom_buf_i = 0;
            SPI_eeprom_read_request = 0;
            break;
    }
}

/*
void eeprom_write(u8 byte)
{
	u16 read;

	CS_EEPROM = 0;

	//write enable
	SPI1BUF = 0x06;
	while (SPI1STATbits.SPIBUSY);
	eeprom_read_byte = SPI1BUF;

	CS_EEPROM = 1;

	//wait for write enable ok
	//read_status_register RDSR : 0x05
	SPI1CONbits.MODE16 = 1;
	SPI1BUF = 0x0500;
	while (SPI1STATbits.SPIBUSY);
	eeprom_read_byte = SPI1BUF;
	SPI1CONbits.MODE16 = 0;


	//write instruction
	CS_EEPROM = 0;

	SPI1BUF = 0x02;
	while (SPI1STATbits.SPIBUSY);
	eeprom_read_byte = SPI1BUF;

	//write address
	SPI1CONbits.MODE16 = 1;
	SPI1BUF = 0x0000;
	while (SPI1STATbits.SPIBUSY);
	read = SPI1BUF;
	SPI1CONbits.MODE16 = 0;

	//write data

	SPI1BUF = byte;
	while (SPI1STATbits.SPIBUSY);
	read = SPI1BUF;

	CS_EEPROM = 1;
}

void eeprom_read(u16 add)
{
	u16 read;

	CS_EEPROM = 0;

	SPI1BUF = 0x03;
	while (SPI1STATbits.SPIBUSY);
	eeprom_read_byte = SPI1BUF;

	SPI1CONbits.MODE16 = 1;
	SPI1BUF = add;
	while (SPI1STATbits.SPIBUSY);
	read = SPI1BUF;
	SPI1CONbits.MODE16 = 0;

	SPI1BUF = 0x00;
	while (SPI1STATbits.SPIBUSY);
	eeprom_read_byte = SPI1BUF;

	CS_EEPROM = 1;
}
*/

void SPI1_eeprom_state_machine(void)
{
    if (SPI_eeprom_read_request)
	eeprom_state_machine_read();
    else if (SPI_eeprom_write_request)
	eeprom_state_machine_write();
//	switch (SPI1_state)
//	{
//		case E_SPI1_EEPROM_WRITE_ENABLE:
//			eeprom_state_machine_write();
//			break;
//		case E_SPI1_EEPROM_READ_ENABLE:
//			eeprom_state_machine_read();
//			break;
//	}
}