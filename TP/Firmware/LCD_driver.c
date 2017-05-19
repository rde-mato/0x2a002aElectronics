#include <xc.h>
#include <sys/attribs.h>
#include "0x2a002a.h"


// GPIO MCP =
// GPA                         GPB
// 7 6 5 4 3   2   1 0         7   to 0
// x x x x CS2 CS1 E DIouRS    DB7 to DB0

#define DI_BIT      0x01
#define ENABLE_BIT 0x02
#define CS1_bit 0x04
#define CS2_bit 0x08


void LCD_display_control_instruction(u8 di, u8 cs1, u8 cs2, u8 db)
{
    u16 cpt = 0;
    u8 conf_bits;

    //chip select + remontee du enable
    conf_bits = ENABLE_BIT | (cs1 != 0) * CS1_bit | (cs2 != 0) * CS2_bit | (di != 0) * DI_BIT;
    MCP_write_8_gpio_a(conf_bits);
    while (++cpt < 4000);
    MCP_write_8_gpio_b(db);
    while (++cpt < 8000);
    conf_bits &= ~(ENABLE_BIT);
    MCP_write_8_gpio_a(conf_bits);
    while (++cpt < 6000);

//    mcp_gpios = db | (rs << 8) | (0b111 << 9) ; // rajouter rw si / quand il sera cable !!!
//    SPI2_transmit32bits(0x40120000 | mcp_gpios);
//    mcp_gpios &= ~(1 << 9); // enable back to 1
//    SPI2_transmit32bits(0x40120000 | mcp_gpios);
//    rs = 0; //juste pour breakpoint
}

void LCD_display_on_off(u8 on)
{
//    on = (on != 0);
    LCD_display_control_instruction(0, 1, 0, (0x3E | on));
}

void LCD_display_start_origin(u8 line)
{
    LCD_display_control_instruction(0, 1, 0, 0xC0 | (line & 0x3F));
}

void LCD_display_set_y_address(u8 y)
{
    LCD_display_control_instruction(0, 1, 0, 0x40 | (y & 0x3F));
}

void LCD_display_set_x_page(u8 x)
{
    LCD_display_control_instruction(0, 1, 0, 0xB8 | (x & 0x3));
}

void LCD_display_write_data(u8 data)
{
    LCD_display_control_instruction(1, 1, 0, data);
}