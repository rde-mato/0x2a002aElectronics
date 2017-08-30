#include <xc.h>
#include "0x2a002a.h"

extern struct s_all all;

void	init_all_struct(void)
{
	all.cur_mode = E_MODE_PATTERN;
	all.playing = 0;
	all.qtime = 0;
	all.bpm_x100 = INITIAL_BPM_x100;
	all.cur_instrument = 0;
	all.cur_pattern = 0;
	all.cur_note = 36;
	all.cur_octave = 3;
	all.cur_velocity = 100;
	all.cur_encoder = 3;
	all.current_leds_on = 0;
	all.active_instruments_u16 = 1;
	all.pattern_in_pastebin = 0;
	all.current_key_scan = 0;
	all.leds_base_case = 0;
	all.SD_initialized = 0;
	all.default_template = TEMPLATE_DEFAULT;
	all.cur_template = TEMPLATE_DEFAULT;
	memset(all.encoders_values, 0, sizeof(all.encoders_values));
	memset(all.active_pattern_per_instrument, 0, sizeof(all.active_pattern_per_instrument));
	memset(all.lcd_chars, ' ', sizeof(all.lcd_chars));
	memory_init();
}

void	GPIO_and_PPS_init(void)
{
	__builtin_disable_interrupts();

	SYSKEY = 0x0;
	SYSKEY = 0xAA996655;
	SYSKEY = 0x556699AA;
	CFGCONbits.IOLOCK = 0;
	SYSKEY = 0x33333333;

	ANSELA = 0;
	ANSELB = 0; // disable all analog ports

	pps_init_encoders();
	pps_init_SPI1();
	pps_init_UART();
	gpio_init_SD_card();

	SYSKEY = 0x0;
	SYSKEY = 0xAA996655;
	SYSKEY = 0x556699AA;
	CFGCONbits.IOLOCK = 1;
	SYSKEY = 0x33333333;
}

void	TIMER_init(void)
{
	timer_1_init();
	timer_2_init();
	timer_3_init();
	timer_4_init();
	timer_5_init();
}

void	INT_init(void)
{
	__builtin_disable_interrupts();
	INTCONbits.MVEC = 1;

	int_init_HT16_press();
	int_init_main_encoder();
	int_init_UART();
	int_init_I2C1();
	int_init_SPI1();
	int_init_MCP_encoders();
	int_init_timer2();
	int_init_timer3();
	int_init_timer4();
	int_init_timer5();

	__builtin_enable_interrupts();
}
