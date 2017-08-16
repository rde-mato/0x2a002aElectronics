#include <xc.h>
#include <sys/attribs.h>
#include "0x2a002a.h"


u32					SD_return; // trouver un nom explicite - je ne sais plus ce que ca fait
u8					SD_error = 0;
static u8				SD_type = SD_SDSC;
static u32				SD_block_number = 0;
static u16				SD_rw_length;
static u16				SD_rw_index = 0;
static u8 			  	*SD_rw_ptr;
static generic_callback			SD_rw_callback = NULL;

void	send_lots_clk(void)
{
	u32	read32;
	u8	i = 20;

	SPI1CONbits.MODE32 = 1;
	while (--i)
	{
		SPI1BUF = 0xFFFFFFFF;
		while (SPI1STATbits.SPIBUSY) ;
		read32 = SPI1BUF;
	}
	SPI1CONbits.MODE32 = 0;
}

u8	SD_command_retry(u8 cmd, u32 arg, u8 crc, u8 restype)
{
	u8	read8;
	u32	read32;
	u8	ret = 0xFF;
	u32	retries = SD_RETRIES;

	CS_SD = 0x0;
	SPI1BUF = 0x40 | cmd;
	while (SPI1STATbits.SPIBUSY) ;
	read8 = SPI1BUF;

	SPI1CONbits.MODE32 = 1;

	SPI1BUF = arg;
	while (SPI1STATbits.SPIBUSY) ;
	read32 = SPI1BUF;

	SPI1CONbits.MODE32 = 0;

	SPI1BUF = crc;
	while (SPI1STATbits.SPIBUSY) ;
	read8 = SPI1BUF;

	while (retries--)
	{
		SPI1BUF = 0xFF;
		while (SPI1STATbits.SPIBUSY) ;
		if ((ret = SPI1BUF) != 0xFF)
			break;
	}
	if (ret != 0xFF && (restype == SD_R3 || restype == SD_R7))
	{
		SPI1CONbits.MODE32 = 1;
		SPI1BUF = 0xFFFFFFFF;
		while (SPI1STATbits.SPIBUSY) ;
		SD_return = SPI1BUF;
		SPI1CONbits.MODE32 = 0;
	}

	CS_SD = 0x1;
	SPI1BUF = 0xFF;
	while (SPI1STATbits.SPIBUSY) ;
	read8 = SPI1BUF;
	return (ret);
}

void	SD_card_init(void)
{
	u8	R1_response;
	u32	retries = SD_RETRIES;

	// baisser le baud rate du spi
	// SPI1BRG = 9; //divise la frequence de SCK par 20 = 2 * (9 + 1)
	SPI1BRG = 3; //divise la frequence de SCK par 6 = 2 * (2 + 1)
	send_lots_clk();
	R1_response = SD_command_retry(0, 0x0, 0x95, SD_R1);
	R1_response = SD_command_retry(8, 0x1AA, 0x87, SD_R7);
	R1_response = SD_command_retry(58, 0x0, 0x01, SD_R3); // reads the OCR register to know the voltage and therefore if the card is SDSC, SDHC or SDXC

	while (retries--)
	{
		R1_response = SD_command_retry(55, 0x0, 0x65, SD_R1);
		R1_response = SD_command_retry(41, 0x0, 0xE5, SD_R1);
		if (R1_response == 0x00)
			break ;
	}
	R1_response = SD_command_retry(58, 0x0, 0x01, SD_R3); // reads the OCR register, if bit 30 of OCR is 0 then SDSC else SDHC or SDXC
	if (SD_return & (1 << 30))
		SD_type = SD_SDHC;
	R1_response = SD_command_retry(16, SD_BLOCK_SIZE, 0x01, SD_R1); // set block length to 512
	// remonter le baud rate du spi
	SPI1BRG = 0; //set baudrate 1Mhz suivant 8 Mhz du pbclk
}

void	SM_SD_read_block_callback(void)
{
	static u8	state = E_SPI1_SDCARD_READ_INIT;
	u8 			read8;
	u32			read32;
	static u32		retries;

	if (!SD_IS_PRESENT)
	{
		read8 = SPI1BUF;
		SPI1_release();
		state = E_SPI1_SDCARD_READ_INIT;
		SD_rw_index = 0;
		SPI1Fbits.SD_r = 0;
		request_template(TEMPLATE_SD_ERROR);
		CS_SD = CS_LINE_UP;
	}

	switch (state)
	{
		case E_SPI1_SDCARD_READ_INIT:
			read8 = SPI1BUF;
			CS_SD = CS_LINE_DOWN;
			SPI1BUF = SD_CMD_READ_SINGLE_BLOCK;
			state = E_SPI1_SDCARD_READ_SEND_BLOCK_ADDRESS;
			SPI1_TRANSMIT_ENABLE = INT_ENABLED;
			break;

		case E_SPI1_SDCARD_READ_SEND_BLOCK_ADDRESS:
			read8 = SPI1BUF;
			SPI1CONbits.MODE32 = 1;
			if (SD_type == SD_SDSC)
				SD_block_number *= SD_BLOCK_SIZE;
			SPI1BUF = SD_block_number;
			state = E_SPI1_SDCARD_READ_SEND_DUMMY_CRC_AND_START_RETRIES;
			SPI1_TRANSMIT_ENABLE = INT_ENABLED;
			break;

		case E_SPI1_SDCARD_READ_SEND_DUMMY_CRC_AND_START_RETRIES:
			read32 = SPI1BUF;
			SPI1CONbits.MODE32 = 0;
			SPI1BUF = SD_CMD_DUMMY_CRC;
			retries = SD_RETRIES;
			state = E_SPI1_SDCARD_READ_RETRY_TILL_NOT_BUSY;
			SPI1_TRANSMIT_ENABLE = INT_ENABLED;
			break;

		case E_SPI1_SDCARD_READ_RETRY_TILL_NOT_BUSY:
			read8 = SPI1BUF;
			if (read8 == 0x00)
			{
				retries = SD_RETRIES;
				state = E_SPI1_SDCARD_READ_RETRY_TILL_TOKEN;
				SPI1BUF = 0xFF;
			}
			else if (retries)
			{
				SPI1BUF = 0xFF;
				retries--;
			}
			else
			{
				state = E_SPI1_SDCARD_READ_ERROR;
				SD_error = SD_READ_ERROR_TOO_MANY_RETRIES;
			}
			SPI1_TRANSMIT_ENABLE = INT_ENABLED;
			break;

		case E_SPI1_SDCARD_READ_RETRY_TILL_TOKEN:
			read8 = SPI1BUF;
			if (read8 != 0xFF)
			{
				if (read8 == 0b11111110)
				{
					state = E_SPI1_SDCARD_READ_GET_ONE_BYTE;
					SPI1BUF = 0xFF;
				}
				else
				{
					state = E_SPI1_SDCARD_READ_ERROR;
					SD_error = SD_READ_ERROR_WRONG_TOKEN ;
				}
			}
			else if (retries)
			{
				SPI1BUF = 0xFF;
				retries--;
			}
			else
			{
				state = E_SPI1_SDCARD_READ_ERROR;
				SD_error = SD_READ_ERROR_TOO_MANY_RETRIES;
			}
			SPI1_TRANSMIT_ENABLE = INT_ENABLED;
			break;

		case E_SPI1_SDCARD_READ_GET_ONE_BYTE:
			read8 = SPI1BUF;
			if (SD_rw_index >= SD_BLOCK_SIZE || SD_rw_index >= SD_rw_length)
				state = E_SPI1_SDCARD_READ_SEND_FINAL_FFS_1;
			else
				SD_rw_ptr[SD_rw_index++] = read8;
			SPI1BUF = 0xFF;
			SPI1_TRANSMIT_ENABLE = INT_ENABLED;
			break;

		case E_SPI1_SDCARD_READ_SEND_FINAL_FFS_1:
			read8 = SPI1BUF;
			SPI1BUF = 0xFF;
			state = E_SPI1_SDCARD_READ_SEND_FINAL_FFS_2;
			SPI1_TRANSMIT_ENABLE = INT_ENABLED;
			break;

		case E_SPI1_SDCARD_READ_SEND_FINAL_FFS_2:
			read8 = SPI1BUF;
			SPI1BUF = 0xFF;
			state = E_SPI1_SDCARD_READ_RELASE;
			SPI1_TRANSMIT_ENABLE = INT_ENABLED;
			break;

		case E_SPI1_SDCARD_READ_RELASE:
			read8 = SPI1BUF;
			CS_SD = CS_LINE_UP;
			SPI1BUF = 0xFF;
			state = E_SPI1_SDCARD_READ_LAST_FF;
			SPI1_TRANSMIT_ENABLE = INT_ENABLED;
			break;

		case E_SPI1_SDCARD_READ_ERROR:
			read8 = SPI1BUF;
			CS_SD = CS_LINE_UP;
			SPI1_release();
			state = E_SPI1_SDCARD_READ_INIT;
			request_template(TEMPLATE_SD_ERROR);
			SPI1Fbits.SD_r = 0;
			break;

		case E_SPI1_SDCARD_READ_LAST_FF:
			read8 = SPI1BUF;
			state = E_SPI1_SDCARD_READ_INIT;
			SPI1_release();
			SD_rw_index = 0;
			SPI1Fbits.SD_r = 0;
			if (SD_rw_callback != NULL)
				(*SD_rw_callback)();
			break;
	}

}


void	SD_read_block_callback(u32 src_block_address, u8 *dest, u16 length, generic_callback cb)
{
	SD_rw_length = length;
	SD_rw_index = 0;
	SD_rw_ptr = dest;
	SD_block_number = src_block_address;
	SD_rw_callback = cb;
	SPI1Fbits.SD_r = 1;
}

//u8	SD_card_read_block(u32 block_address, generic_callback cb)
//{
//	SD_rw_callback = cb;
//	SPI1Fbits.SD_r = 1;
//	SD_block_number = block_address;
//}



void	SM_SD_write_block_callback(void)
{
	static u8		state = E_SPI1_SDCARD_WRITE_INIT;
	static u32		retries;
	u8			read8;
	u32			read32;

	if (!SD_IS_PRESENT)
	{
		read8 = SPI1BUF;
		SD_rw_index = 0;
		SPI1Fbits.SD_w = 0;
		state = E_SPI1_SDCARD_WRITE_INIT;
		SPI1_release();
		request_template(TEMPLATE_SD_ERROR);
		CS_SD = CS_LINE_UP;
	}

	switch (state)
	{
		case E_SPI1_SDCARD_WRITE_INIT:
			read8 = SPI1BUF;
			CS_SD = CS_LINE_DOWN;
			SPI1BUF = SD_CMD_WRITE_SINGLE_BLOCK;
			state = E_SPI1_SDCARD_WRITE_SEND_BLOCK_ADDRESS;
			SPI1_TRANSMIT_ENABLE = INT_ENABLED;
			break;

		case E_SPI1_SDCARD_WRITE_SEND_BLOCK_ADDRESS:
			read8 = SPI1BUF;
			SPI1CONbits.MODE32 = 1;
			if (SD_type == SD_SDSC)
				SD_block_number *= SD_BLOCK_SIZE;
			SPI1BUF = SD_block_number;
			state = E_SPI1_SDCARD_WRITE_SEND_DUMMY_CRC_AND_START_RETRIES;
			SPI1_TRANSMIT_ENABLE = INT_ENABLED;
			break;

		case E_SPI1_SDCARD_WRITE_SEND_DUMMY_CRC_AND_START_RETRIES:
			read32 = SPI1BUF;
			SPI1CONbits.MODE32 = 0;
			SPI1BUF = SD_CMD_DUMMY_CRC;
			retries = SD_RETRIES;
			state = E_SPI1_SDCARD_WRITE_RETRY_TILL_NOT_BUSY;
			SPI1_TRANSMIT_ENABLE = INT_ENABLED;
			break;


		case E_SPI1_SDCARD_WRITE_RETRY_TILL_NOT_BUSY:
			read8 = SPI1BUF;
			if (read8 == 0x00)
			{
				retries = SD_RETRIES;
				state = E_SPI1_SDCARD_WRITE_WAIT_ONE_BYTE;
				SPI1BUF = 0xFF;
			}
			else if (retries)
			{
				SPI1BUF = 0xFF;
				retries--;
			}
			else
			{
				state = E_SPI1_SDCARD_WRITE_ERROR;
				SD_error = SD_WRITE_ERROR_TOO_MANY_RETRIES;
			}
			SPI1_TRANSMIT_ENABLE = INT_ENABLED;
			break;


		case E_SPI1_SDCARD_WRITE_WAIT_ONE_BYTE:
			read8 = SPI1BUF;
			SPI1BUF = 0xFF;
			state = E_SPI1_SDCARD_WRITE_SEND_START_TOKEN;
			SPI1_TRANSMIT_ENABLE = INT_ENABLED;
			break;


		case E_SPI1_SDCARD_WRITE_SEND_START_TOKEN:
			read8 = SPI1BUF;
			SPI1BUF = 0b11111110;
			state = E_SPI1_SDCARD_WRITE_SEND_ONE_BYTE;
			SPI1_TRANSMIT_ENABLE = INT_ENABLED;
			break;


		case E_SPI1_SDCARD_WRITE_SEND_ONE_BYTE:
			read8 = SPI1BUF;
			SPI1BUF = SD_rw_ptr[SD_rw_index++];
			if (SD_rw_index >= SD_BLOCK_SIZE || SD_rw_index >= SD_rw_length)
			{
				retries = SD_RETRIES;
				state = E_SPI1_SDCARD_WRITE_RETRY_TILL_DATA_RESPONSE;
			}
			SPI1_TRANSMIT_ENABLE = INT_ENABLED;
			break;


		case E_SPI1_SDCARD_WRITE_RETRY_TILL_DATA_RESPONSE:
			read8 = SPI1BUF;
			if (((read8 >> 4) & 1) == 0 && (read8 & 1) == 1)
			{
				if ((read8 & 0b11111) == 0b0101)
				{
					SPI1BUF = 0xFF;
					state = E_SPI1_SDCARD_WRITE_SEND_FF_TILL_END_OF_00;
				}
				else if ((read8 & 0b11111) == 0b1011)
				{
					state = E_SPI1_SDCARD_WRITE_ERROR;
					SD_error = SD_WRITE_ERROR_WRONG_DATA_RESPONSE; // a ameliorer
				}
				else if ((read8 & 0b11111) == 0b1101)
				{
					state = E_SPI1_SDCARD_WRITE_ERROR;
					SD_error = SD_WRITE_ERROR_WRONG_DATA_RESPONSE; // a ameliorer
				}
				else
				{
					state = E_SPI1_SDCARD_WRITE_ERROR;
					SD_error = SD_WRITE_ERROR_WRONG_DATA_RESPONSE; // a ameliorer
				}
			}
			else if (retries)
			{
				SPI1BUF = 0xFF;
				retries--;
			}
			else
			{
				state = E_SPI1_SDCARD_WRITE_ERROR;
				SD_error = SD_WRITE_ERROR_TOO_MANY_RETRIES;
			}
			SPI1_TRANSMIT_ENABLE = INT_ENABLED;
			break;

		case E_SPI1_SDCARD_WRITE_SEND_FF_TILL_END_OF_00:
			read8 = SPI1BUF;
			if (read8 == 0x00)
			{
				state = E_SPI1_SDCARD_WRITE_RELASE;
				SPI1BUF = 0xFF;
			}
			else if (retries)
			{
				SPI1BUF = 0xFF;
				retries--;
			}
			else
			{
				state = E_SPI1_SDCARD_WRITE_ERROR;
				SD_error = SD_WRITE_ERROR_TOO_MANY_RETRIES;
			}
			SPI1_TRANSMIT_ENABLE = INT_ENABLED;
			break;


		case E_SPI1_SDCARD_WRITE_RELASE:
			read8 = SPI1BUF;
			CS_SD = CS_LINE_UP;
			SPI1BUF = 0xFF;
			state = E_SPI1_SDCARD_WRITE_LAST_FF;
			SPI1_TRANSMIT_ENABLE = INT_ENABLED;
			break;

		case E_SPI1_SDCARD_WRITE_ERROR:
			read8 = SPI1BUF;
			CS_SD = CS_LINE_UP;
			// SPI1_state = E_SPI1_DONE;
			// request_template(TEMPLATE_SD_ERROR);
			// SPI_SDCARD_write_request = 0;
			state = E_SPI1_SDCARD_WRITE_INIT;
			SD_rw_index = 0;
			SPI1_TRANSMIT_ENABLE = INT_ENABLED;
			//
			break;

		case E_SPI1_SDCARD_WRITE_LAST_FF:
			read8 = SPI1BUF;
			state = E_SPI1_SDCARD_WRITE_INIT;
			SPI1_release();
			SD_rw_index = 0;
			SPI1Fbits.SD_w = 0;
			if (SD_rw_callback != NULL)
				(*SD_rw_callback)();
			break;

	}
}

void	SD_write_block_callback(u8 *src, u32 dest_block_address, u16 length, generic_callback cb)
{
	SD_rw_length = length;
	SD_rw_index = 0;
	SD_rw_ptr = src;
	SD_block_number = dest_block_address;
	SD_rw_callback = cb;
	SPI1Fbits.SD_w = 1;
}


//u8	SD_card_write_block(u32 block_address, generic_callback cb)
//{
//	SD_rw_callback = cb;
//	SPI1Fbits.SD_w = 1;
//	SD_block_number = block_address;
//}

void	gpio_init_SD_card(void)
{
	SD_PRESENCE_GPIO = GPIO_INPUT;
}
