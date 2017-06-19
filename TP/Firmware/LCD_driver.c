#include <xc.h>
#include <sys/attribs.h>
#include "0x2a002a.h"

#include "charset.h"
#define MAX_LINES 8
#define MAX_CHAR_LINE 20


#define LCD_RIGHT 0
#define LCD_LEFT 1

u8	lcd_chars[8][21] = { 0 };
u8	lcd_buffer[8][128] = { 0 };


void LCD_display_control_instruction(u8 cs1, u8 cs2, u8 di, u8 rw, u8 data)
{
	u16 send;

        send = (LCD_ENABLE_BIT | (rw != 0) * LCD_RW_BIT | (cs1 != 0) * LCD_CS1_bit | (cs2 != 0) * LCD_CS2_bit | (di != 0) * LCD_DI_BIT) << 8 | data;
        SPI2BUF = send;
        while (!SPI2STATbits.SPIRBF) ;
        SPI2BUF = send & ~(LCD_ENABLE_BIT << 8);
        while (!SPI2STATbits.SPIRBF) ;
}

u16 LCD_instruction_to_enable_low(u8 cs1, u8 cs2, u8 di, u8 rw, u8 data)
{
    return (((rw != 0) * LCD_RW_BIT | (cs1 != 0) * LCD_CS1_bit | (cs2 != 0) * LCD_CS2_bit | (di != 0) * LCD_DI_BIT) << 8 | data);
}

void    LCD_clear(void)
{
    u8  line = 0;
    u8  col;

        SS_MCP_LCD = 0x0;
        SPI2BUF = 0x4012;
        while (!SPI2STATbits.SPIRBF)
            ;
        while (line < 8)
        {
            LCD_display_control_instruction(1, 1, 0, 0, 0b10111000 | line++);   //    LCD_display_set_x_page;
            col = 0;
            LCD_display_control_instruction(1, 1, 0, 0, 0b01000000 | col);      //    LCD_display_set_y_address(0);
            while (col++ < 64)
                LCD_display_control_instruction(1, 1, 1, 0, 0x00);
        }
        SS_MCP_LCD = 0x1;
}