#include <xc.h>
#include <sys/attribs.h>
#include "0x2a002a.h"

#include "charset.h"
#define MAX_LINES 8
#define MAX_CHAR_LINE 20

// GPIO MCP =
// GPA                         GPB
// 7 6 5 4 3   2   1 0         7   to 0
// x x x x CS2 CS1 E DIouRS    DB7 to DB0

#define DI_BIT      0x01
#define ENABLE_BIT 0x02
#define CS1_bit 0x04
#define CS2_bit 0x08

#define LCD_LEFT 0
#define HALF_LENGTH 100

u8	lcd_line = 0;
u8	lcd_col = 0;
u8	lcd_half = LCD_LEFT;

inline void LCD_display_control_instruction(u8 di, u8 cs1, u8 cs2, u8 db)
{
	u8 conf_bits;

	conf_bits = ENABLE_BIT | (cs1 != 0) * CS1_bit | (cs2 != 0) * CS2_bit | (di != 0) * DI_BIT;
	MCP_write_8_gpio_a(conf_bits);
	MCP_write_8_gpio_b(db);
	conf_bits &= ~(ENABLE_BIT);
	MCP_write_8_gpio_a(conf_bits);
	conf_bits &= 0x00 | (di != 0) * DI_BIT;
	MCP_write_8_gpio_a(conf_bits);
}

void LCD_display_on_off(u8 on)
{
	on = (on != 0);
	LCD_display_control_instruction(0, 1, 1, (0x3E | on));
}

void LCD_display_start_origin(u8 line)
{
	LCD_display_control_instruction(0, 1, 1, 0xC0 | (line & 0x3F));
}

//void LCD_display_set_y_address(u8 y)
//{
//	LCD_display_control_instruction(0, 1, 1, 0x40 | (y & 0x3F));
//}

void LCD_display_set_y_address(u8 left, u8 y)
{
	left = (left != 0);
	LCD_display_control_instruction(0, left, 1 - left, 0x40 | (y & 0x3F));
}

//void LCD_display_set_x_page(u8 x)
//{
//	LCD_display_control_instruction(0, 1, 1, 0xB8 | (x & 0x7));
//}

void LCD_display_set_x_page(u8 left, u8 x)
{
	left = (left != 0);
	LCD_display_control_instruction(0, left, 1 - left, 0xB8 | (x & 0x7));
}

void LCD_display_write_data(u8 data)
{
	LCD_display_control_instruction(1, 1, 1, data);
}

//void LCD_display_write_data(u8 left, u8 data)
//{
//	left = (left != 0);
//	LCD_display_control_instruction(1, left, 1 - left, data);
//}

void LCD_clear(void)
{
	u8 x = -1;
	u8 y = -1;

	LCD_display_start_origin(0);
	LCD_display_set_y_address(1, 0);
	LCD_display_set_x_page(1, 0);
	while (++y < 8)
	{
		LCD_display_set_x_page(1, y);
		x = -1;
		while (++x < 64)
			LCD_display_write_data(0);
	}
        LCD_display_start_origin(0);
	LCD_display_set_y_address(0, 0);
	LCD_display_set_x_page(0, 0);
	while (++y < 8)
	{
		LCD_display_set_x_page(0, y);
		x = -1;
		while (++x < 64)
			LCD_display_write_data(0);
	}
}

void    ftputchar(char c)
{
    int i;

    for(i = 0; i < 5; ++i)
    {
        LCD_display_write_data(charset[c][i]);
    }
}

void    ftputstr(char *str)
{
    while(*str)
    {
        ftputchar(*str);
        ++str;
    }
}