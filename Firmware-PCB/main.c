#include <xc.h>
#include <sys/attribs.h>
#include "0x2a002a.h"

int main(void)
{

    //    u8 ret1;
    //    u8 ret2;
    //    u8 ret3;
    //    u32 block = 2200;

    GPIO_and_PPS_init();
    TIMER_init();
    I2C1_init();
    SPI1_init();

    MCP_LCD_init();
    LCD_init();
    MCP_ENCODERS_init_blocking();
    UART1_init();

    //		SD_card_init();
    //		ret1 = SD_card_read_block(block);
    //		ret2 = SD_card_write_block(block);
    //		ret3 = SD_card_read_block(block);

    INT_init();
    HT16_init();

    set_bpm(DEFAULT_BPM);
    request_template(&template_default);

    IFS0bits.INT1IF = 1; // a garder pour reset les encoders

    update_leds_base_case();

    while (42)
    {
        I2C1_manager();
        SPI1_manager();
        WDTCONbits.WDTCLR = 1; // CLEAR WATCHDOG
    }
    return (0);
}
