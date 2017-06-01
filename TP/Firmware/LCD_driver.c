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

#define LCD_RIGHT 0
#define LCD_LEFT 1
#define HALF_LENGTH 100

u8	lcd_chars[8][21] = { 0 };
u8	lcd_buffer[8][128] = { 0 };

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

void LCD_display_start_origin(u8 side, u8 line)
{
	LCD_display_control_instruction(0,
			(side == LCD_LEFT),
			(side == LCD_RIGHT),
			0xC0 | (line & 0x3F));
}

void LCD_display_set_y_address(u8 side, u8 y)
{
	LCD_display_control_instruction(0,
			(side == LCD_LEFT),
			(side == LCD_RIGHT),
			0x40 | (y & 0x3F));
}

void LCD_display_set_x_page(u8 side, u8 x)
{
	LCD_display_control_instruction(0,
			(side == LCD_LEFT),
			(side == LCD_RIGHT),
			0xB8 | (x & 0x7));
}

void LCD_display_write_data(u8 side, u8 data)
{
	LCD_display_control_instruction(0,
			(side == LCD_LEFT),
			(side == LCD_RIGHT),
			data);
}

void	clear_LCD_buffer(void)
{
	u8	line;
	u8	col;

	line = 0;
	while (line < 8)
	{
		col = 0;
		while (col < 128)
			lcd_buffer[line][col++] = 0;
		col++;
	}
}

void	LCD_chars_to_buffer(void)
{
	u8	line;
	u8	char_index;
	u8	char_col;

	line = 0;
	while (line < 8)
	{
		char_index = 0;
		while (char_index < 21)
		{
			char_col = 0;
			while (char_col < 6)
			{
				lcd_buffer[line][6 * char_index + char_col] =
					charset[lcd_chars[line][char_index]][char_col];
				++char_col;
			}
			++char_index;
		}
		++line;
	}
}

void	print_LCD_buffer(void)
{
	u8	line;
	u8	col;

	line = 0;
	while (line < 8)
	{
		col = 0;
		LCD_display_start_origin(LCD_LEFT, 0);
		LCD_display_set_x_page(LCD_LEFT, line);
		LCD_display_set_y_address(LCD_LEFT, col);
		while (col < 64)
			LCD_display_write_data(LCD_LEFT, lcd_buffer[line][col++]);
		if (col == 64)
		{
			LCD_display_start_origin(LCD_RIGHT, 0);
			LCD_display_set_x_page(LCD_RIGHT, line);
			LCD_display_set_y_address(LCD_RIGHT, col - 64);
			while (col < 128)
				LCD_display_write_data(LCD_RIGHT, lcd_buffer[line][col++]);
		}
		++line;
	}
}

void LCD_clear(void)
{
	clear_LCD_buffer();
	print_LCD_buffer();

	//	u8 x = -1;
	//	u8 y = -1;
	//
	//	LCD_display_start_origin(0);
	//	LCD_display_set_y_address(1, 0);
	//	LCD_display_set_x_page(1, 0);
	//	while (++y < 8)
	//	{
	//		LCD_display_set_x_page(1, y);
	//		x = -1;
	//		while (++x < 64)
	//			LCD_display_write_data(0);
	//	}
	//	LCD_display_start_origin(0);
	//	LCD_display_set_y_address(0, 0);
	//	LCD_display_set_x_page(0, 0);
	//	while (++y < 8)
	//	{
	//		LCD_display_set_x_page(0, y);
	//		x = -1;
	//		while (++x < 64)
	//			LCD_display_write_data(0);
	//	}
}

void    ftputchar(char c)
{
	int i;

	for(i = 0; i < 5; ++i)
	{
		LCD_display_write_data(charset[c][i]);
	}
}

void    ft_putline(u8 line, char *str)
{
	u8	i;

	if (line >= 8)
		return;
	i = 0;
	while(i < 21 && str[i])
	{
		lcd_chars[line][i] = str[i];
		++i;
	}
	LCD_chars_to_buffer();
	print_LCD_buffer();
}

void    ftputstr(char *str)
{
	while(*str)
	{
		ftputchar(*str);
		++str;
	}
}
