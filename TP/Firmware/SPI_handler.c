#include <xc.h>
#include <sys/attribs.h>
#include "0x2a002a.h"

extern  u8  MCP_LCD_SPI_request;
extern  u8  MCP_ENC_SPI_request;
extern  u8  SD_SPI_request;
extern  u8  FLASH_SPI_request;


void    manage_SPI2(void)
{
	if (MCP_LCD_SPI_request)
		write_LCD();
//	else if (MCP_ENC_SPI_request)
//		read_encoders();
//	else if (SD_SPI_request)
//		write_sd();
//	else if (FLASH_SPI_request)
//		write_flash();
}
