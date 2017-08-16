#include <xc.h>
#include <sys/attribs.h>
#include "0x2a002a.h"

static u16				eeprom_rw_address;
static u16				eeprom_rw_length;
static u16				eeprom_rw_index = 0;
static u8 			  	*eeprom_rw_ptr;
static generic_callback	eeprom_rw_callback = NULL;


void	enable_spi_interrupt(void)
{
	SPI1_TRANSMIT_ENABLE = INT_ENABLED;
}

void	SM_eeprom_write_callback(void)
{
	static u8	state =  E_SPI1_EEPROM_CHECK_BUSY;
	u8			read8;

	switch (state)
	{
		case E_SPI1_EEPROM_CHECK_BUSY:
			read8 = SPI1BUF;
			CS_EEPROM = CS_LINE_DOWN;
			SPI1BUF = E_EEPROM_RDSR;
			state = E_SPI1_EEPROM_CHECK_BUSY_2;
			SPI1_TRANSMIT_ENABLE = INT_ENABLED;
			break ;

		case E_SPI1_EEPROM_CHECK_BUSY_2:
			read8 = SPI1BUF;
			SPI1BUF = 0xFF;
			state = E_SPI1_EEPROM_CHECK_BUSY_3;
			SPI1_TRANSMIT_ENABLE = INT_ENABLED;
			break ;

		case E_SPI1_EEPROM_CHECK_BUSY_3:
			read8 = SPI1BUF;
			SPI1BUF = 0xFF;
			if (((read8 ^ 0b10) & 0b01) == 0)
				state = E_SPI1_EEPROM_WRITE_ENABLE_1;
			else
				state = E_SPI1_EEPROM_CHECK_BUSY;
			CS_EEPROM = CS_LINE_UP;
			SPI1_TRANSMIT_ENABLE = INT_ENABLED;
			break ;

		case E_SPI1_EEPROM_WRITE_ENABLE_1:
			read8 = SPI1BUF;
			CS_EEPROM = CS_LINE_DOWN;
			SPI1BUF = E_EEPROM_WREN;
			state = E_SPI1_EEPROM_WRITE_ENABLE_2;
			SPI1_TRANSMIT_ENABLE = INT_ENABLED;
			break ;

		case E_SPI1_EEPROM_WRITE_ENABLE_2:
			read8 = SPI1BUF;
			SPI1BUF = 0xFF;
			CS_EEPROM = CS_LINE_UP;
			state = E_SPI1_EEPROM_CHECK_WREN;
			SPI1_TRANSMIT_ENABLE = INT_ENABLED;
			break ;

		case E_SPI1_EEPROM_CHECK_WREN:
			read8 = SPI1BUF;
			CS_EEPROM = CS_LINE_DOWN;
			SPI1BUF = E_EEPROM_RDSR;
			state = E_SPI1_EEPROM_CHECK_WREN_2;
			SPI1_TRANSMIT_ENABLE = INT_ENABLED;
			break ;

		case E_SPI1_EEPROM_CHECK_WREN_2:
			read8 = SPI1BUF;
			SPI1BUF = 0xFF;
			state = E_SPI1_EEPROM_CHECK_WREN_3;
			SPI1_TRANSMIT_ENABLE = INT_ENABLED;
			break ;

		case E_SPI1_EEPROM_CHECK_WREN_3:
			read8 = SPI1BUF;
			SPI1BUF = 0xFF;
			if (((read8 ^ 0b10) & 0b11) == 0)
				state = E_SPI1_EEPROM_WRITE_INSTRUCTION;
			else
				state = E_SPI1_EEPROM_WRITE_ENABLE_1;
			CS_EEPROM = CS_LINE_UP;
			SPI1_TRANSMIT_ENABLE = INT_ENABLED;
			break ;

		case E_SPI1_EEPROM_WRITE_INSTRUCTION:
			CS_EEPROM = CS_LINE_DOWN;
			read8 = SPI1BUF;
			state = E_SPI1_EEPROM_WRITE_ADDRESS_FIRST_BYTE;
			SPI1BUF = E_EEPROM_WRITE;
			SPI1_TRANSMIT_ENABLE = INT_ENABLED;
			break ;

		case E_SPI1_EEPROM_WRITE_ADDRESS_FIRST_BYTE:
			read8 = SPI1BUF;
			state = E_SPI1_EEPROM_WRITE_ADDRESS_SECOND_BYTE;
			SPI1BUF = (u8)(eeprom_rw_address >> 8);
			SPI1_TRANSMIT_ENABLE = INT_ENABLED;
			break ;

		case E_SPI1_EEPROM_WRITE_ADDRESS_SECOND_BYTE:
			read8 = SPI1BUF;
			state = E_SPI1_EEPROM_WRITE_DATA;
			SPI1BUF = (u8)(eeprom_rw_address);
			SPI1_TRANSMIT_ENABLE = INT_ENABLED;
			break ;

		case E_SPI1_EEPROM_WRITE_DATA:
			read8 = SPI1BUF;
			if (eeprom_rw_index >= eeprom_rw_length - 1 || (eeprom_rw_address & 127) == 127)
			{
				state = E_SPI1_EEPROM_WRITE_PAGE_DONE;
				SPI1BUF = eeprom_rw_ptr[eeprom_rw_index++];
				SPI1_TRANSMIT_ENABLE = INT_ENABLED;
			}
			else
			{
				SPI1BUF = eeprom_rw_ptr[eeprom_rw_index++];
				SPI1_TRANSMIT_ENABLE = INT_ENABLED;
			}
			eeprom_rw_address++;
			break ;

		case E_SPI1_EEPROM_WRITE_PAGE_DONE:
			read8 = SPI1BUF;
			CS_EEPROM = CS_LINE_UP;
			if (eeprom_rw_index >= eeprom_rw_length)
				state = E_SPI1_EEPROM_WRITE_CALLBACK;
			else
				state = E_SPI1_EEPROM_CHECK_BUSY;
			timer4_push(25, &enable_spi_interrupt);
	    		break;

		case E_SPI1_EEPROM_WRITE_CALLBACK:

			eeprom_rw_length = 0;
			eeprom_rw_index = 0;
			SPI1Fbits.eeprom_w = FALSE;
			SPI1_release();
			state =  E_SPI1_EEPROM_CHECK_BUSY;
			if (eeprom_rw_callback != NULL)
				(*eeprom_rw_callback)();
			break;
	}
}

void	eeprom_write_callback(u8 *src, u16 dest_address, u16 length, generic_callback cb)
{
	eeprom_rw_address = dest_address;
	eeprom_rw_length = length;
	eeprom_rw_ptr = src;
	eeprom_rw_callback = cb;
	SPI1Fbits.eeprom_w = TRUE;
}

void	SM_eeprom_read_callback(void)
{
	static u8	state = E_SPI1_EEPROM_READ_INSTRUCTION;
	u8  		read8;

	switch (state)
	{
		case E_SPI1_EEPROM_READ_INSTRUCTION:
			read8 = SPI1BUF;
			CS_EEPROM = CS_LINE_DOWN;
			state = E_SPI1_EEPROM_READ_ADDRESS_FIRST_BYTE;
			SPI1BUF = E_EEPROM_READ;
			SPI1_TRANSMIT_ENABLE = INT_ENABLED;
			break ;

		case E_SPI1_EEPROM_READ_ADDRESS_FIRST_BYTE:
			read8 = SPI1BUF;
			state = E_SPI1_EEPROM_READ_ADDRESS_SECOND_BYTE;
			SPI1BUF = (u8)(eeprom_rw_address >> 8);
			SPI1_TRANSMIT_ENABLE = INT_ENABLED;
			break ;

		case E_SPI1_EEPROM_READ_ADDRESS_SECOND_BYTE:
			read8 = SPI1BUF;
			state = E_SPI1_EEPROM_READ_ADDRESS_START_READ;
			SPI1BUF = (u8)(eeprom_rw_address);
			SPI1_TRANSMIT_ENABLE = INT_ENABLED;
			break ;

		case E_SPI1_EEPROM_READ_ADDRESS_START_READ:
			read8 = SPI1BUF;
			state = E_SPI1_EEPROM_READ_DATA;
			SPI1BUF = 0xFF;
			SPI1_TRANSMIT_ENABLE = INT_ENABLED;
			break ;

		case E_SPI1_EEPROM_READ_DATA:
			eeprom_rw_ptr[eeprom_rw_index++] = SPI1BUF;
			if (eeprom_rw_index >= eeprom_rw_length)
				state = E_SPI1_EEPROM_READ_CALLBACK;
			else
			{
				SPI1BUF = 0xFF;
				SPI1_RECEIVE_ENABLE = INT_ENABLED;
				break ;
			}

		case E_SPI1_EEPROM_READ_CALLBACK:
			CS_EEPROM = CS_LINE_UP;
			SPI1_release();
			eeprom_rw_length = 0;
			eeprom_rw_index = 0;
			SPI1Fbits.eeprom_r = FALSE;
			state =  E_SPI1_EEPROM_READ_INSTRUCTION;
			if (eeprom_rw_callback != NULL)
				(*eeprom_rw_callback)();
			break;
	}
}

void	eeprom_read_callback(u16 src_address, u8 *dest, u16 length, generic_callback cb)
{
	eeprom_rw_address = src_address;
	eeprom_rw_length = length;
	eeprom_rw_callback = cb;
	eeprom_rw_ptr = dest;
	SPI1Fbits.eeprom_r = TRUE;
}

void	eeprom_init(void)
{
	u8 read;

	CS_EEPROM = CS_LINE_DOWN;
	SPI1BUF = E_EEPROM_WREN;
	while (SPI1STATbits.SPIBUSY);
	read = SPI1BUF;
	CS_EEPROM = CS_LINE_UP;
}

void	eeprom_chip_erase(void)
{
	u8  read;

	CS_EEPROM = CS_LINE_DOWN;
	SPI1BUF = E_EEPROM_WREN;
	while (SPI1STATbits.SPIBUSY);
	read = SPI1BUF;
	CS_EEPROM = CS_LINE_UP;

	CS_EEPROM = CS_LINE_DOWN;
	SPI1BUF = E_EEPROM_CE;
	while (SPI1STATbits.SPIBUSY);
	read = SPI1BUF;
	CS_EEPROM = CS_LINE_UP;

	memory_init();
	sequencer_pause();
}
