#include <xc.h>
#include <sys/attribs.h>
#include "0x2a002a.h"

char pattern[16][4][3];

int main(void)
{
    u16 cpt = 0;

    init_pattern();

    GPIO_init();
    TIMER_init();
    I2C2_init();
    SPI2_init();
    
    INT_init();

    HT16_init();
   // MCP_init();
    //MCP_write(0x04, 0xFFFF);
    T2CONbits.ON = 1;

    led_toggle(0);

    MCP_init_LCD();

    LCD_display_on_off(1);
    while (++cpt < 4000) ;
    LCD_display_start_origin(16);
    while (++cpt < 8000) ;
    LCD_display_set_y_address(16);
    while (++cpt < 12000) ;
    LCD_display_set_x_page(16);
    while (++cpt < 16000) ;
    LCD_display_write_data(0xAA);


    while (42)
    {
        manage_I2C2();
        WDTCONbits.WDTCLR = 1; // CLEAR WATCHDOG
    }
    return (0);
}
