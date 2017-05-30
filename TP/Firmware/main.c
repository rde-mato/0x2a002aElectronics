#include <xc.h>
#include <sys/attribs.h>
#include "0x2a002a.h"

char pattern[16][4][3];

int main(void)
{
    u8 i = 0;
    u8 y = -1;
    
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
    LCD_clear();
    LCD_display_start_origin(0);
    LCD_display_set_y_address(0);
    LCD_display_set_x_page(0);


    

    while (42)
    {
        y = -1;
        while (++y < 8)
        {
            LCD_display_set_x_page(y);
            i = -1;
            while (++i < 64)
                LCD_display_write_data(0xaa);
        }
        y = -1;
        while (++y < 8)
        {
            LCD_display_set_x_page(y);
            i = -1;
            while (++i < 64)
                LCD_display_write_data(~0xaa);
        }
        manage_I2C2();
        WDTCONbits.WDTCLR = 1; // CLEAR WATCHDOG
    }
    return (0);
}
