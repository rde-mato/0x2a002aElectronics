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
    TEMPLATE_PATTERN,
    TEMPLATE_ENCODER,
    TEMPLATE_MAIN_MENU,
    TEMPLATE_BPM,
    TEMPLATE_VELOCITY,
    TEMPLATE_INSTRUMENT,
    TEMPLATE_OCTAVE,
    TEMPLATE_PATTERN_RECORDED,
    TEMPLATE_LOADING_STARTED,
    TEMPLATE_CHIP_ERASED,
    TEMPLATE_LOADING_SUCCESSFUL,
    TEMPLATE_SD_ERROR
};

enum E_MENU_ITEMS
{
    E_MENU_ITEMS_PASTE_PATTERN,
    E_MENU_ITEMS_COPY_PATTERN,
    E_MENU_ITEMS_LOAD_FROM_SD,
    E_MENU_ITEMS_LOAD_TO_SD,
    E_MENU_ITEMS_CLEAR_EEPROM,
    E_MENU_ITEMS_EXIT
};

#endif	/* LCD_H */