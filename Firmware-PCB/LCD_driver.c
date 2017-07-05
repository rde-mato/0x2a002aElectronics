#include <xc.h>
#include <sys/attribs.h>
#include "0x2a002a.h"

#include "charset.h"
#define MAX_LINES 8
#define MAX_CHAR_LINE 21


#define LCD_RIGHT 0
#define LCD_LEFT 1

u8	lcd_chars[8][21] = { ' ' };
u8      lcd_changed_chars[8][21] = { 0 };

u8      lcd_line_ptr = 0;
u8      lcd_col_ptr = 1;
u8      lcd_pos_ptr = 0;
u8      lcd_current_side = LCD_LEFT;

void LCD_blocking_control_instruction(u8 cs1, u8 cs2, u8 di, u8 rw, u8 data)
{
	u16 send;
        u16 clear;

        send = (LCD_ENABLE_BIT | (rw != 0) * LCD_RW_BIT | (cs1 != 0) * LCD_CS1_bit | (cs2 != 0) * LCD_CS2_bit | (di != 0) * LCD_DI_BIT) << 8 | data;
        SPI1BUF = send;
        while (!SPI1STATbits.SPIRBF) ;
        clear = SPI1BUF;
        SPI1BUF = send & ~(LCD_ENABLE_BIT << 8);
        while (!SPI1STATbits.SPIRBF) ;
        clear = SPI1BUF;
}

u16 LCD_instruction_to_enable_low(u8 cs1, u8 cs2, u8 di, u8 rw, u8 data)
{
    return (((rw != 0) * LCD_RW_BIT | (cs1 != 0) * LCD_CS1_bit | (cs2 != 0) * LCD_CS2_bit | (di != 0) * LCD_DI_BIT) << 8 | data);
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
        lcd_col_ptr = ++col & 63;
    }
    else if (line != lcd_line_ptr)
    {
        buffer[index++] = LCD_display_set_x_page(lcd_current_side, line);
        buffer[index++] = LCD_display_set_y_address(lcd_current_side, col & 63);
        buffer[index++] = LCD_display_write_data(lcd_current_side, data);
        lcd_line_ptr = line;
        lcd_col_ptr = ++col  & 63;
    }
    else if (col != lcd_col_ptr)
    {
        buffer[index++] = LCD_display_set_y_address(lcd_current_side, col & 63);
        buffer[index++] = LCD_display_write_data(lcd_current_side, data);
        lcd_col_ptr = ++col & 63;
    }
    else
    {
        buffer[index++] = LCD_display_write_data(lcd_current_side, data);
        lcd_col_ptr = ++col & 63;
    }
    SPI1_push_LCD_buffer(buffer, index);
}

void    LCD_print_char(u8 line, u8 pos, u8 c)
{
    u8  i = 0;

    if (line > 8 || pos > 21)
        return ;
    pos = 6 * pos + 1;
    while (i < 6)
        LCD_print_u8(line, pos++, (c < 128) ? charset[c][i++] : ~(charset[c - 128][i++]));
    // si le caractere est superieur a 127, alors c'est la "diapositive" de l'autre
    // peut etre avantageux de modifier le charset directement ...
}

void    LCD_print_changed_chars(void)
{
    u8  line = 0;
    u8  pos;

    while (line < 8)
    {
        pos = 0;
        while (pos < 21)
        {
            if (lcd_changed_chars[line][pos])
            {
                lcd_changed_chars[line][pos] = 0;
                LCD_print_char(line, pos, lcd_chars[line][pos]);
            }
            ++pos;
        }
        ++line;
    }
}

void    LCD_putchar(u8 line, u8 pos, u8 c)
{
    if (lcd_chars[line][pos] != c)
    {
        lcd_chars[line][pos] = c;
        lcd_changed_chars[line][pos] = 1;
    }
}

void    LCD_clear(void)
{
    u8  line = 0;
    u8  pos = 0;

    while (line < 8)
    {
        pos = 0;
        while (pos < 21)
            LCD_putchar(line, pos++, ' ');
        line++;
    }
}

void    LCD_putnstr(u8 line, u8 pos, u8 *str, u8 size)
{
    while (*str && size--)
        LCD_putchar(line, pos++, *(str++));
}

void    LCD_putstr(u8 line, u8 pos, u8 *str)
{
    while (*str)
        LCD_putchar(line, pos++, *(str++));
//    while ((*str != '\0') && (line < 8))
//    {
//        while ((pos <= 21) && (*str != '\0') && (*str != '\n') && (*str != '\n' + 128))
//            LCD_putchar(line, pos++, *(str++));
//        while (pos <= 21)
//            LCD_putchar(line, pos++, *str);
//        if ((*str != '\n') || (*str != '\n' + 128))
//            str++;
//        line++;
//        pos = 0;
//    }
}

void    LCD_putstr_negative(u8 line, u8 pos, u8 *str)
{
    while (*str)
        LCD_putchar(line, pos++, *(str++) + 0x80);
}