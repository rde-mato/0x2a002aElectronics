#include <xc.h>
#include <sys/attribs.h>
#include "0x2a002a.h"

int main(void)
{

    u32 block = 0;

    GPIO_and_PPS_init();
    TIMER_init();
    I2C1_init();
    SPI1_init();

    MCP_LCD_init();
    LCD_init();
    MCP_ENCODERS_init_blocking();
    UART1_init();
//    eeprom_chip_erase();
    no_notes_everywhere();
    initial_eeprom_download(); // this starts first template
    //TODO: EEPROM loads strange data in active_patterns and current_pattern,
//    if (SD_IS_PRESENT)
        SD_card_init();
//    ret2 = SD_card_write_block(block);
//    ret3 = SD_card_read_block(block);

    INT_init();
    HT16_init();
    IFS0bits.INT1IF = 1; // a garder pour reset les encoders
    TIMER2_ON;


//    SD_card_read_block(block);
    SD_card_write_block(block);

    while (42)
    {
        I2C1_manager();
        SPI1_manager();
        CLEAR_WATCHDOG;
    }
    return (0);
}
