#include <xc.h>
#include <sys/attribs.h>
#include "0x2a002a.h"
#include <string.h>

static u8	SPI1_action = E_SPI1_ACTION_NONE;

void	SPI1_init(void)
{
	SPI1CON = 0;
	SPI1BUF = 0;
	SPI1BRG = 0; //set baudrate 1Mhz suivant 8 Mhz du pbclk
	SPI1CONbits.CKE = 1;
	SPI1CONbits.CKP = 0; // mode 00
	SPI1CONbits.MODE16 = 0;
	SPI1CONbits.MODE32 = 0;
	SPI1CONbits.MSTEN = 1;
	SPI1CONbits.ON = 1;

	memset(&SPI1Fbits, 0, sizeof(SPI1Fbits)); // initialisation of SPI flags
}

void	pps_init_SPI1(void)
{
	SPI1_CS0_GPIO = GPIO_OUTPUT;
	CS_MCP_LCD = CS_LINE_UP;
	SPI1_CS1_GPIO = GPIO_OUTPUT;
	CS_MCP_ENCODERS = CS_LINE_UP;
	SPI1_CS2_GPIO = GPIO_OUTPUT;
	CS_EEPROM = CS_LINE_UP;
	SPI1_CS3_GPIO = GPIO_OUTPUT;
	CS_SD = CS_LINE_UP;
	PPS_SDI1;
	PPS_SDO1;
}

void	int_init_SPI1(void)
{
	SPI1_INT_FLAGS_CLR_RX;
	SPI1_INT_FLAGS_CLR_TX;
	SPI1_INT_PRIORITIES = 4;
	SPI1_RECEIVE_ENABLE = INT_DISABLED;
	SPI1_TRANSMIT_ENABLE = INT_DISABLED;
}

void	__ISR(_SPI_1_VECTOR, IPL4AUTO) SPI1Handler(void)
{
	SPI1_RECEIVE_ENABLE = INT_DISABLED;
	SPI1_TRANSMIT_ENABLE = INT_DISABLED;

	switch (SPI1_action)
	{
		case E_SPI1_ACTION_LCD_WRITE:
			SM_SPI1_LCD();
			break;
		case E_SPI1_ACTION_ENCODERS_READ:
			SM_SPI1_encoders();
			break;
		case E_SPI1_ACTION_EEPROM_READ:
			SM_eeprom_read_callback();
			break;
		case E_SPI1_ACTION_EEPROM_WRITE:
			SM_eeprom_write_callback();
			break;
		case E_SPI1_ACTION_SD_READ:
			SM_SD_read_block_callback();
			break;
		case E_SPI1_ACTION_SD_WRITE:
			SM_SD_write_block_callback();
			break;
	}
}

inline void	SPI1_set_priority(u8 action)
{
	SPI1_action = action;
}

inline void	SPI1_priority_manager(void)
{
	if (!SPI1_READY)
		return ;

	if (SPI1Fbits.MCP_encoders_r)
		SPI1_set_priority(E_SPI1_ACTION_ENCODERS_READ);
	else if (SPI1Fbits.LCD_w)
	{
		SPI1Fbits.LCD_w = 0;
		display_current_template(); // a mettre ailleurs
		SPI1_set_priority(E_SPI1_ACTION_LCD_WRITE);
	}
	else if (SPI1Fbits.eeprom_w)
		SPI1_set_priority(E_SPI1_ACTION_EEPROM_WRITE);
	else if (SPI1Fbits.eeprom_r)
		SPI1_set_priority(E_SPI1_ACTION_EEPROM_READ);
	else if (SPI1Fbits.SD_r)
		SPI1_set_priority(E_SPI1_ACTION_SD_READ);
	else if (SPI1Fbits.SD_w)
		SPI1_set_priority(E_SPI1_ACTION_SD_WRITE);

	if (SPI1_action != E_SPI1_ACTION_NONE)
		SPI1_TRANSMIT_ENABLE = INT_ENABLED;
}

inline void	SPI1_release(void)
{
	SPI1_set_priority(E_SPI1_ACTION_NONE);
}
