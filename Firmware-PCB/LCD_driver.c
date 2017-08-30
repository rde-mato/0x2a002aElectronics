#include <xc.h>
#include <sys/attribs.h>
#include "0x2a002a.h"

extern struct s_all	all;

static u32			SPI_LCD_count = 0;
static u16			SPI_buf_LCD[LCD_BUF_MAX] = { 0 };
static u32			SPI_LCD_index = 0;
static u8			lcd_changed_chars[8][CHARS_PER_LINE] = { 0 };
static u8			lcd_line_ptr = 0;
static u8			lcd_col_ptr = 1;
static u8			lcd_current_side = LCD_LEFT;

const u8	charset[][6] =
{
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x5F, 0x00, 0x00},
	{0x00, 0x00, 0x07, 0x00, 0x07, 0x00},
	{0x00, 0x14, 0x7F, 0x14, 0x7F, 0x14},
	{0x00, 0x24, 0x2A, 0x7F, 0x2A, 0x12},
	{0x00, 0x23, 0x13, 0x08, 0x64, 0x62},
	{0x00, 0x36, 0x49, 0x55, 0x22, 0x50},
	{0x00, 0x00, 0x05, 0x03, 0x00, 0x00},
	{0x00, 0x00, 0x1C, 0x22, 0x41, 0x00},
	{0x00, 0x00, 0x41, 0x22, 0x1C, 0x00},
	{0x00, 0x08, 0x2A, 0x1C, 0x2A, 0x08},
	{0x00, 0x08, 0x08, 0x3E, 0x08, 0x08},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x08, 0x08, 0x08, 0x08, 0x08},
	{0x00, 0x00, 0x30, 0x30, 0x00, 0x00},
	{0x00, 0x20, 0x10, 0x08, 0x04, 0x02},
	{0x00, 0x3E, 0x51, 0x49, 0x45, 0x3E},
	{0x00, 0x00, 0x42, 0x7F, 0x40, 0x00},
	{0x00, 0x42, 0x61, 0x51, 0x49, 0x46},
	{0x00, 0x21, 0x41, 0x45, 0x4B, 0x31},
	{0x00, 0x18, 0x14, 0x12, 0x7F, 0x10},
	{0x00, 0x27, 0x45, 0x45, 0x45, 0x39},
	{0x00, 0x3C, 0x4A, 0x49, 0x49, 0x30},
	{0x00, 0x01, 0x71, 0x09, 0x05, 0x03},
	{0x00, 0x36, 0x49, 0x49, 0x49, 0x36},
	{0x00, 0x06, 0x49, 0x49, 0x29, 0x1E},
	{0x00, 0x00, 0x36, 0x36, 0x00, 0x00},
	{0x00, 0x00, 0x56, 0x36, 0x00, 0x00},
	{0x00, 0x00, 0x08, 0x14, 0x22, 0x41},
	{0x00, 0x14, 0x14, 0x14, 0x14, 0x14},
	{0x00, 0x41, 0x22, 0x14, 0x08, 0x00},
	{0x00, 0x02, 0x01, 0x51, 0x09, 0x06},
	{0x00, 0x32, 0x49, 0x79, 0x41, 0x3E},
	{0x00, 0x7E, 0x11, 0x11, 0x11, 0x7E},
	{0x00, 0x7F, 0x49, 0x49, 0x49, 0x36},
	{0x00, 0x3E, 0x41, 0x41, 0x41, 0x22},
	{0x00, 0x7F, 0x41, 0x41, 0x22, 0x1C},
	{0x00, 0x7F, 0x49, 0x49, 0x49, 0x41},
	{0x00, 0x7F, 0x09, 0x09, 0x01, 0x01},
	{0x00, 0x3E, 0x41, 0x41, 0x51, 0x32},
	{0x00, 0x7F, 0x08, 0x08, 0x08, 0x7F},
	{0x00, 0x00, 0x41, 0x7F, 0x41, 0x00},
	{0x00, 0x20, 0x40, 0x41, 0x3F, 0x01},
	{0x00, 0x7F, 0x08, 0x14, 0x22, 0x41},
	{0x00, 0x7F, 0x40, 0x40, 0x40, 0x40},
	{0x00, 0x7F, 0x02, 0x04, 0x02, 0x7F},
	{0x00, 0x7F, 0x04, 0x08, 0x10, 0x7F},
	{0x00, 0x3E, 0x41, 0x41, 0x41, 0x3E},
	{0x00, 0x7F, 0x09, 0x09, 0x09, 0x06},
	{0x00, 0x3E, 0x41, 0x51, 0x21, 0x5E},
	{0x00, 0x7F, 0x09, 0x19, 0x29, 0x46},
	{0x00, 0x46, 0x49, 0x49, 0x49, 0x31},
	{0x00, 0x01, 0x01, 0x7F, 0x01, 0x01},
	{0x00, 0x3F, 0x40, 0x40, 0x40, 0x3F},
	{0x00, 0x1F, 0x20, 0x40, 0x20, 0x1F},
	{0x00, 0x7F, 0x20, 0x18, 0x20, 0x7F},
	{0x00, 0x63, 0x14, 0x08, 0x14, 0x63},
	{0x00, 0x03, 0x04, 0x78, 0x04, 0x03},
	{0x00, 0x61, 0x51, 0x49, 0x45, 0x43},
	{0x00, 0x00, 0x00, 0x7F, 0x41, 0x41},
	{0x00, 0x02, 0x04, 0x08, 0x10, 0x20},
	{0x00, 0x41, 0x41, 0x7F, 0x00, 0x00},
	{0x00, 0x04, 0x02, 0x01, 0x02, 0x04},
	{0x00, 0x40, 0x40, 0x40, 0x40, 0x40},
	{0x00, 0x00, 0x01, 0x02, 0x04, 0x00},
	{0x00, 0x20, 0x54, 0x54, 0x54, 0x78},
	{0x00, 0x7F, 0x48, 0x44, 0x44, 0x38},
	{0x00, 0x38, 0x44, 0x44, 0x44, 0x20},
	{0x00, 0x38, 0x44, 0x44, 0x48, 0x7F},
	{0x00, 0x38, 0x54, 0x54, 0x54, 0x18},
	{0x00, 0x08, 0x7E, 0x09, 0x01, 0x02},
	{0x00, 0x08, 0x14, 0x54, 0x54, 0x3C},
	{0x00, 0x7F, 0x08, 0x04, 0x04, 0x78},
	{0x00, 0x00, 0x44, 0x7D, 0x40, 0x00},
	{0x00, 0x20, 0x40, 0x44, 0x3D, 0x00},
	{0x00, 0x00, 0x7F, 0x10, 0x28, 0x44},
	{0x00, 0x00, 0x41, 0x7F, 0x40, 0x00},
	{0x00, 0x7C, 0x04, 0x18, 0x04, 0x78},
	{0x00, 0x7C, 0x08, 0x04, 0x04, 0x78},
	{0x00, 0x38, 0x44, 0x44, 0x44, 0x38},
	{0x00, 0x7C, 0x14, 0x14, 0x14, 0x08},
	{0x00, 0x08, 0x14, 0x14, 0x18, 0x7C},
	{0x00, 0x7C, 0x08, 0x04, 0x04, 0x08},
	{0x00, 0x48, 0x54, 0x54, 0x54, 0x20},
	{0x00, 0x04, 0x3F, 0x44, 0x40, 0x20},
	{0x00, 0x3C, 0x40, 0x40, 0x20, 0x7C},
	{0x00, 0x1C, 0x20, 0x40, 0x20, 0x1C},
	{0x00, 0x3C, 0x40, 0x30, 0x40, 0x3C},
	{0x00, 0x44, 0x28, 0x10, 0x28, 0x44},
	{0x00, 0x0C, 0x50, 0x50, 0x50, 0x3C},
	{0x00, 0x44, 0x64, 0x54, 0x4C, 0x44},
	{0x00, 0x00, 0x08, 0x36, 0x41, 0x00},
	{0x00, 0x00, 0x00, 0x7F, 0x00, 0x00},
	{0x00, 0x00, 0x41, 0x36, 0x08, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
};


void	MCP_LCD_init(void)
{
	u16	read;

	SPI1CONbits.MODE16 = 1;

	//pins en output
	CS_MCP_LCD = 0x0;
	SPI1BUF = 0x4000;
	while (SPI1STATbits.SPIBUSY) ;
	read = SPI1BUF;
	SPI1BUF = 0x0000;
	while (SPI1STATbits.SPIBUSY) ;
	read = SPI1BUF;
	CS_MCP_LCD = 0x1;


	//mode sequentiel off
	CS_MCP_LCD = 0x0;
	SPI1BUF = 0x400A;
	while (SPI1STATbits.SPIBUSY) ;
	read = SPI1BUF;
	SPI1BUF = 0x2020;
	while (SPI1STATbits.SPIBUSY) ;
	read = SPI1BUF;
	CS_MCP_LCD = 0x1;

	SPI1CONbits.MODE16 = 0;
}


void	LCD_blocking_control_instruction(u8 cs1, u8 cs2, u8 di, u8 rw, u8 data)
{
	u16	send;
	u16	clear;

	send = (LCD_ENABLE_BIT | (rw != 0) * LCD_RW_BIT | (cs1 != 0) * LCD_CS1_bit | (cs2 != 0) * LCD_CS2_bit | (di != 0) * LCD_DI_BIT) << 8 | data;
	SPI1BUF = send;
	while (!SPI1STATbits.SPIRBF) ;
	clear = SPI1BUF;
	SPI1BUF = send & ~(LCD_ENABLE_BIT << 8);
	while (!SPI1STATbits.SPIRBF) ;
	clear = SPI1BUF;
}

void	LCD_init(void)
{
	u8	line = 0;
	u8	col;
	u16	clear;

	// clear du LCD
	CS_MCP_LCD = 0x0;
	SPI1CONbits.MODE16 = 1;
	SPI1BUF = 0x4012;
	while (SPI1STATbits.SPIBUSY) ;
	clear = SPI1BUF;
	LCD_blocking_control_instruction(1, 1, 0, 0, 0b00111111); // LCD_display_on_off(1);
	LCD_blocking_control_instruction(1, 1, 0, 0, 0b11000000); // LCD_display_start_origin(0);
	while (line < 8)
	{
		LCD_blocking_control_instruction(1, 1, 0, 0, 0b10111000 | line++); // LCD_display_set_x_page;
		col = 0;
		LCD_blocking_control_instruction(1, 1, 0, 0, 0b01000000 | col); // LCD_display_set_y_address(0);
		while (col++ < 64)
			LCD_blocking_control_instruction(1, 1, 1, 0, 0);
	}
	LCD_blocking_control_instruction(1, 1, 0, 0, 0b01000001); // LCD_display_set_y_address(1);
	LCD_blocking_control_instruction(1, 1, 0, 0, 0b10111000); // LCD_display_set_x_page(0);
	SPI1CONbits.MODE16 = 0;
	CS_MCP_LCD = 0x1;
}

u16	LCD_instruction_to_enable_low(u8 cs1, u8 cs2, u8 di, u8 rw, u8 data)
{
	return (((rw != 0) * LCD_RW_BIT | (cs1 != 0) * LCD_CS1_bit | (cs2 != 0) * LCD_CS2_bit | (di != 0) * LCD_DI_BIT) << 8 | data);
}


u16	LCD_display_start_origin(u8 side, u8 line)
{
	return (LCD_instruction_to_enable_low((side == LCD_LEFT), (side == LCD_RIGHT), 0, 0, 0xC0 | (line & 0x3F)));
}

u16 LCD_display_set_y_address(u8 side, u8 y)
{
	return (LCD_instruction_to_enable_low((side == LCD_LEFT), (side == LCD_RIGHT), 0, 0, 0x40 | (y & 0x3F)));
}

u16	LCD_display_set_x_page(u8 side, u8 x)
{
	return (LCD_instruction_to_enable_low((side == LCD_LEFT), (side == LCD_RIGHT), 0, 0, 0xB8 | (x & 0x7)));
}

u16	LCD_display_write_data(u8 side, u8 data)
{
	return (LCD_instruction_to_enable_low((side == LCD_LEFT), (side == LCD_RIGHT), 1, 0, data));
}

void	SPI1_push_LCD_buffer(u16 *buffer, u32 size)
{
	u32	i;

	IEC1bits.SPI1RXIE = 0;
	IEC1bits.SPI1TXIE = 0;
	i = 0;
	while (i < size)
		SPI_buf_LCD[SPI_LCD_count++] = buffer[i++];
	IEC1bits.SPI1RXIE = 1;
	IEC1bits.SPI1TXIE = 1;
}

void	LCD_print_u8(u8 line, u8 col, u8 data)
{
	u16	buffer[3] = { 0 };
	u8	index = 0;

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
		lcd_col_ptr = ++col & 63;
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

void	LCD_print_char(u8 line, u8 pos, u8 c)
{
	u8	i = 0;

	if (line > 8 || pos > CHARS_PER_LINE)
		return ;
	pos = 6 * pos + 1;
	while (i < 6)
		LCD_print_u8(line, pos++, (c < 128) ? charset[c][i++] : ~(charset[c - 128][i++]));
	// si le caractere est superieur a 127, alors c'est la "diapositive" de l'autre
	// peut etre avantageux de modifier le charset directement ...
}

void	LCD_print_changed_chars(void)
{
	u8	line = 0;
	u8	pos;

	while (line < 8)
	{
		pos = 0;
		while (pos < CHARS_PER_LINE)
		{
			if (lcd_changed_chars[line][pos])
			{
				lcd_changed_chars[line][pos] = 0;
				LCD_print_char(line, pos, all.lcd_chars[line][pos]);
			}
			++pos;
		}
		++line;
	}
}

void	LCD_putchar(u8 line, u8 pos, u8 c)
{
	if (all.lcd_chars[line][pos] != c)
	{
		all.lcd_chars[line][pos] = c;
		lcd_changed_chars[line][pos] = 1;
	}
}

void	LCD_clear(void)
{
	u8	line = 0;
	u8	pos = 0;

	while (line < 8)
	{
		pos = 0;
		while (pos < CHARS_PER_LINE)
			LCD_putchar(line, pos++, ' ');
		line++;
	}
}

void	LCD_putnstr(u8 line, u8 pos, u8 *str, u8 size)
{
	while (*str && size--)
		LCD_putchar(line, pos++, *(str++));
}

void	LCD_putstr(u8 line, u8 pos, u8 *str)
{
	while (*str)
		LCD_putchar(line, pos++, *(str++));
}

void	LCD_putstr_negative(u8 line, u8 pos, u8 *str)
{
	while (*str)
		LCD_putchar(line, pos++, *(str++) + 0x80);
}

void	SM_SPI1_LCD(void)
{
	static u8	state = E_SPI1_LCD_CONFIG;
	u32			clear;

	switch (state)
	{
		case E_SPI1_LCD_CONFIG:
			SPI1CONbits.MODE16 = 1;
			CS_MCP_LCD = 0x0;
			state = E_SPI1_LCD_WRITE_ENABLE_HIGH;
			SPI1BUF = LCD_PORTS_ADDRESS;
			SPI1_RECEIVE_ENABLE = INT_ENABLED;
			SPI1_TRANSMIT_ENABLE = INT_ENABLED;
			break;
		case E_SPI1_LCD_WRITE_ENABLE_LOW:
			if (SPI1STATbits.SPIBUSY)
				break ;
			clear = SPI1BUF;
			state = E_SPI1_LCD_WRITE_ENABLE_HIGH;
			SPI1BUF = SPI_buf_LCD[SPI_LCD_index++];
			SPI1_RECEIVE_ENABLE = INT_ENABLED;
			SPI1_TRANSMIT_ENABLE = INT_ENABLED;
			break ;
		case E_SPI1_LCD_WRITE_ENABLE_HIGH:
			if (SPI1STATbits.SPIBUSY)
				break ;
			if (SPI_LCD_index < SPI_LCD_count)
			{
				clear = SPI1BUF;
				state = E_SPI1_LCD_WRITE_ENABLE_LOW;
				SPI1BUF = SPI_buf_LCD[SPI_LCD_index] | (LCD_ENABLE_BIT << 8);
				SPI1_RECEIVE_ENABLE = INT_ENABLED;
				SPI1_TRANSMIT_ENABLE = INT_ENABLED;
				break ;
			}
			else
				state = E_SPI1_LCD_RELEASE;
		case E_SPI1_LCD_RELEASE:
			clear = SPI1BUF;
			state = E_SPI1_LCD_CONFIG;
			SPI1_release();
			SPI1CONbits.MODE16 = 0;
			CS_MCP_LCD = 0x1;
			SPI_LCD_index = 0;
			SPI_LCD_count = 0;
			break;
	}
}
