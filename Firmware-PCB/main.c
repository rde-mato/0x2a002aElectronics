#include <xc.h>
#include <sys/attribs.h>
#include "0x2a002a.h"

void			UART1_push(char *buf, char size);
u8				sdhere = 0;
midi_callback	midi_send = UART1_push;

int	main(void)
{
	GPIO_and_PPS_init();
	TIMER_init();
	I2C1_init();
	SPI1_init();

	MCP_LCD_init();
	LCD_init();
	MCP_ENCODERS_init_blocking();
	UART1_init();
	memory_init(); // a regrouper dans une init de l'eeprom ?
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
