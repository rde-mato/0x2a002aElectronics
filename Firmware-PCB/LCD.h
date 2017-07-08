#ifndef LCD_H
#define	LCD_H

#define MAX_LINES           8
#define MAX_CHAR_LINE       21
#define LCD_RIGHT           0
#define LCD_LEFT            1

#define LCD_BUF_MAX         2048  //128 * ( 8 + 2 )


// GPIO MCP =
// GPA                                  GPB
// 7  6  5  4    3    2  1   0           7   to 0
// x  x  x  CS2  CS1  E  RW  DIouRS      DB7 to DB0

#define LCD_DI_BIT          0x01
#define LCD_RW_BIT          0x02
#define LCD_ENABLE_BIT      0x04
#define LCD_CS1_bit         0x08
#define LCD_CS2_bit         0x10
#define LCD_PORTS_ADDRESS   0x4012
#define LINES_COUNT          8
#define CHARS_PER_LINE       21

enum E_LCD_TEMPLATES
{
    TEMPLATE_DEFAULT = 0,
    TEMPLATE_NOTE,
    TEMPLATE_ENCODER,
    TEMPLATE_MAIN_MENU_START,
    TEMPLATE_BPM,
    TEMPLATE_VELOCITY,
    TEMPLATE_OCTAVE
};

#endif	/* LCD_H */

