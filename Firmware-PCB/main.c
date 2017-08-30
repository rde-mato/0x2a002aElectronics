#include <xc.h>
#include <sys/attribs.h>
#include "0x2a002a.h"

struct s_all	all;

int	main(void)
{
	init_all_struct();
	GPIO_and_PPS_init();
	TIMER_init();
	I2C1_init();
	SPI1_init();

	MCP_LCD_init();
	LCD_init();
	MCP_ENCODERS_init_blocking();
	UART1_init();
	initial_eeprom_download();
	request_template(TEMPLATE_DEFAULT);
	set_bpm();

	INT_init();
	HT16_init();
	IFS0bits.INT1IF = 1; // a garder pour reset les encoders
	TIMER2_ON;

	while (42)
	{
		I2C1_manager();
		SPI1_priority_manager();
		CLEAR_WATCHDOG;
	}
	return (0);
}
