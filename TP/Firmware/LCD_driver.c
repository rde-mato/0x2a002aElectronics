#include <xc.h>
#include <sys/attribs.h>
#include "0x2a002a.h"

#include "charset.h"
#define MAX_LINES 8
#define MAX_CHAR_LINE 20


#define LCD_RIGHT 0
#define LCD_LEFT 1

u8	lcd_chars[8][21] = { 0 };
u8      lcd_changed_chars[8][21] = { 0 };
u16     lcd_changed_chars_count = 0;

u8	lcd_buffer[8][128] = { 0 };
u8      lcd_line_ptr = 0;
u8      lcd_col_ptr = 0;
u8      lcd_current_side = LCD_LEFT;

void LCD_blocking_control_instruction(u8 cs1, u8 cs2, u8 di, u8 rw, u8 data)
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
            LCD_blocking_control_instruction(1, 1, 0, 0, 0b10111000 | line++);   //    LCD_display_set_x_page;
            col = 0;
            LCD_blocking_control_instruction(1, 1, 0, 0, 0b01000000 | col);      //    LCD_display_set_y_address(0);
            while (col++ < 64)
                LCD_blocking_control_instruction(1, 1, 1, 0, 0x00);
        }
        LCD_blocking_control_instruction(1, 1, 0, 0, 0b01000000); //    LCD_display_set_y_address(0);
        LCD_blocking_control_instruction(1, 1, 0, 0, 0b10111000); //    LCD_display_set_x_page(0);
        SS_MCP_LCD = 0x1;
}



u16 LCD_display_start_origin(u8 side, u8 line)
{
	return (LCD_instruction_to_enable_low((side == LCD_LEFT),
                                        (side == LCD_RIGHT),
                                        0,
                                        0,
                            		0xC0 | (line & 0x3F)));
}

u16 LCD_display_set_y_address(u8 side, u8 y)
{
	return (LCD_instruction_to_enable_low((side == LCD_LEFT),
                                	(side == LCD_RIGHT),
                                        0,
                                        0,
                                	0x40 | (y & 0x3F)));
}

u16 LCD_display_set_x_page(u8 side, u8 x)
{
	return (LCD_instruction_to_enable_low((side == LCD_LEFT),
                                        (side == LCD_RIGHT),
                                        0,
                                        0,
                                        0xB8 | (x & 0x7)));
}

u16 LCD_display_write_data(u8 side, u8 data)
{
	return (LCD_instruction_to_enable_low((side == LCD_LEFT),
                                        (side == LCD_RIGHT),
                                        1,
                                        0,
                                        data));
}

void    LCD_print_u8(u8 line, u8 col, u8 data)
{
    u16 buffer[3] = { 0 };
    u8  index = 0;

    if ((col < 64 && lcd_current_side == LCD_RIGHT) || (col >= 64 && lcd_current_side == LCD_LEFT))
    {
        lcd_current_side = (col < 64) ? LCD_LEFT : LCD_RIGHT;
        buffer[index++] = LCD_display_set_x_page(lcd_current_side, line);
        buffer[index++] = LCD_display_set_y_address(lcd_current_side, col & 63);
        buffer[index++] = LCD_display_write_data(lcd_current_side, data);
        lcd_line_ptr = line;
        lcd_col_ptr = ++col;
    }
    else if (line != lcd_line_ptr)
    {
        buffer[index++] = LCD_display_set_x_page(lcd_current_side, line);
        buffer[index++] = LCD_display_set_y_address(lcd_current_side, col & 63);
        buffer[index++] = LCD_display_write_data(lcd_current_side, data);
        lcd_line_ptr = line;
        lcd_col_ptr = ++col;
    }
    else if (col != lcd_col_ptr)
    {
        buffer[index++] = LCD_display_set_y_address(lcd_current_side, col & 63);
        buffer[index++] = LCD_display_write_data(lcd_current_side, data);
        lcd_col_ptr = ++col;
    }
    else
    {
        buffer[index++] = LCD_display_write_data(lcd_current_side, data);
        lcd_col_ptr = ++col;
    }
    SPI2_push_LCD_buffer(buffer, index);
}
//void	LCD_chars_to_buffer(void)
//{
//	u8	line;
//	u8	char_index;
//	u8	char_col;
//
//	line = 0;
//	while (line < 8)
//	{
//		char_index = 0;
//		while (char_index < 21)
//		{
//			char_col = 0;
//			while (char_col < 6)
//			{
//				lcd_buffer[line][6 * char_index + char_col] =
//					charset[lcd_chars[line][char_index]][char_col];
//				++char_col;
//			}
//			++char_index;
//		}
//		++line;
//	}
//}
//
//void	print_LCD_buffer(void)
//{
//	u8	line;
//	u8	col;
//
//	line = 0;
//	while (line < 8)
//	{
//		col = 0;
//		LCD_display_start_origin(LCD_LEFT, 0);
//		LCD_display_set_x_page(LCD_LEFT, line);
//		LCD_display_set_y_address(LCD_LEFT, col);
//		while (col < 64)
//			LCD_display_write_data(LCD_LEFT, lcd_buffer[line][col++]);
//		if (col == 64)
//		{
//			LCD_display_start_origin(LCD_RIGHT, 0);
//			LCD_display_set_x_page(LCD_RIGHT, line);
//			LCD_display_set_y_address(LCD_RIGHT, col - 64);
//			while (col < 128)
//				LCD_display_write_data(LCD_RIGHT, lcd_buffer[line][col++]);
//		}
//		++line;
//	}
//}
//
//void    LCD_load_line(u8 line, char *str)
//{
//	u8	i;
//
//	if (line >= 8)
//		return;
//	i = 0;
//	while(i < 21 && str[i])
//	{
//		lcd_chars[line][i] = str[i];
//		++i;
//	}
//        while(i < 21)
//	{
//		lcd_chars[line][i] = '\0';
//		++i;
//	}
//}
//
//void	print_LCD_chars(void)
//{
//       LCD_chars_to_buffer();
//       print_LCD_buffer();
//}