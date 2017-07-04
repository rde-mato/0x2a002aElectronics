#include <xc.h>
#include <sys/attribs.h>
#include "0x2a002a.h"

#define INSTRUMENTS_COUNT       16
#define PATTERNS_PER_INSTRUMENT 16
#define QTIME_PER_INSTRUMENT    16
#define NOTES_PER_QTIME         4
#define ATTRIBUTES_PER_NOTE     3

//  1 pattern = 192 u8
//  1 instrument = 3072 u8

u8	active_patterns[INSTRUMENTS_COUNT][QTIME_PER_INSTRUMENT][NOTES_PER_QTIME][ATTRIBUTES_PER_NOTE];
u8	active_instrument[PATTERNS_PER_INSTRUMENT][QTIME_PER_INSTRUMENT][NOTES_PER_QTIME][ATTRIBUTES_PER_NOTE];
u8      pattern_pointers[INSTRUMENTS_COUNT][PATTERNS_PER_INSTRUMENT];

u8	cur_instrument;
u8	cur_pattern;
u8	cur_note;
u8	cur_octave;
u8	cur_velocity;
u8	bpm;

u8	encoders_memory[16][8];
u8	encoders_play[16][8];
//u16	notes_memory[16][16][16][4]; //trop grand pour etre dans la memoire mais a aller taper direct dans la flash
u16	notes_play[16][16][4];
u16	active_instruments;
extern u8 eeprom_read_byte;


extern u16		eeprom_address;
extern u8               eeprom_buf_i;
extern u8		eeprom_buf_size;
extern u8		eeprom_buf[128];
extern u8     SPI_eeprom_write_request;
extern u8     SPI_eeprom_read_request;
//	pour chaque instrument actif on ne garde que le pattern actif
//

u8  led = 0;

int main(void)
{
	GPIO_and_PPS_init();
	TIMER_init();
	I2C1_init();
	SPI1_init();

	MCP_LCD_init();
	LCD_init();
	MCP_ENCODERS_init_blocking();
//        UART1_init();
//
//	SD_card_init();
//	ret = SD_card_read_block(block);
//	ret = SD_card_write_block(block);
//	ret = SD_card_read_block(block);
//
	INT_init();
	HT16_init();

        eeprom_buf_size = 1;
        eeprom_buf[0] = 0x2a;
        SPI_eeprom_write_request = 1;
        manage_SPI1();


        led_set(0);
        set_bpm(DEFAULT_BPM);

	T2CONbits.ON = 1;
        IFS0bits.INT1IF = 1; // a garder pour reset les encoders


//        char    line[22] = "12345678901";
//
//        LCD_putstr(0, 0, line);
//        LCD_print_changed_chars();
        u16 ih;
        for (ih=0; ih< 42420; ++ih);
        eeprom_buf[0] = 0x0;
        eeprom_buf_size = 1;
        SPI_eeprom_read_request = 1;
        manage_SPI1();
        for (ih=0; ih< 42420; ++ih);
        LCD_putchar(0, 0, eeprom_buf[0]);
        LCD_print_changed_chars();




	while (42)
	{
//            UART1_send(42);
            manage_I2C1();
            manage_SPI1();
            WDTCONbits.WDTCLR = 1; // CLEAR WATCHDOG
	}
	return (0);
}
