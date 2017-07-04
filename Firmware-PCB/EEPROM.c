#include <xc.h>
#include <sys/attribs.h>
#include "0x2a002a.h"

u8		eeprom_read_byte = 0;
u16		eeprom_address;

void	eeprom_init()
{
    CS_EEPROM = 0;



    CS_EEPROM = 1;
}

void	eeprom_state_machine_read(void)
{
    u16	eeprom_read_byte;

	switch (SPI1_state)
	{
	case E_SPI1_EEPROM_WRITE_ENABLE:
    	CS_EEPROM = 0;
		SPI1BUF = 0x06;
		SPI1_state = E_SPI1_EEPROM_WRITE_ENABLE_OK;

		SPI1_RECEIVE_ENABLE = INT_ENABLED;
		SPI1_TRANSFER_ENABLE = INT_ENABLED;
		break;
	case E_SPI1_EEPROM_WRITE_ENABLE_OK:
		if (SPI1STATbits.SPIBUSY)
			break;
		eeprom_read_byte = SPI1BUF;
    	CS_EEPROM = 1;
		SPI1_state = E_SPI1_EEPROM_WRITE_MODE;

		SPI1_RECEIVE_ENABLE = INT_ENABLED;
		SPI1_TRANSFER_ENABLE = INT_ENABLED;
		break;

	case E_SPI1_EEPROM_WRITE_MODE:
		if (SPI1STATbits.SPIBUSY)
			break;
    	CS_EEPROM = 0;
    	SPI1BUF = 0x02;
		SPI1_state = E_SPI1_EEPROM_WRITE_ADDRESS;

		SPI1_RECEIVE_ENABLE = INT_ENABLED;
		SPI1_TRANSFER_ENABLE = INT_ENABLED;
		break;

	case E_SPI1_EEPROM_WRITE_ADDRESS:
		if (SPI1STATbits.SPIBUSY)
			break;
		SPI1CONbits.MODE16 = 1;
		SPI1BUF = 0x0000;

		SPI1_state = E_SPI1_EEPROM_WRITE_DATA;

		SPI1_RECEIVE_ENABLE = INT_ENABLED;
		SPI1_TRANSFER_ENABLE = INT_ENABLED;
	case E_SPI1_EEPROM_WRITE_DATA:
		if (SPI1STATbits.SPIBUSY)
			break;
		read = SPI1BUF;
		SPI1CONbits.MODE16 = 0;
		break;
	}
}

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