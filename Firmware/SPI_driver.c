#include <xc.h>
#include <sys/attribs.h>
#include "0x2a002a.h"

#define SLAVE_COUNT 4
#define LCD_BUF_MAX 30 //128 * ( 8 + 2 )
#define FLASH_BUF_MAX 30                        // calculs a faire


u8  SPI2_state = E_SPI2_DONE;
u8  SPI2_slave = E_SPI2_SS_MCP_LCD;

u16	SPI_buf_LCD[LCD_BUF_MAX] = { 0 };
u32	SPI_LCD_index = 0;
u32	SPI_LCD_count = 0;

u16	SPI_buf_FLASH[FLASH_BUF_MAX] = { 0 };
u32     SPI_flash_index = 0;
u32     SPI_flash_count = 0;


void __ISR(_SPI_2_VECTOR, IPL4AUTO) SPI2Handler(void)           // a voir apres comment faire plusieurs state machines en fonction du slave
{
    IFS1bits.SPI2RXIF = 0;
    IFS1bits.SPI2TXIF = 0;
    
    switch (SPI2_state)
    {
        case E_SPI2_LCD_CONFIG:
            SPI2CONbits.MODE16 = 1;
            SS_MCP_LCD = 0x0;
            SPI2_state = E_SPI2_LCD_WRITE_ENABLE_HIGH;
            SPI2BUF = LCD_PORTS_ADDRESS;
            break;
        case E_SPI2_LCD_WRITE_ENABLE_LOW:
            if (SPI2STATbits.SPIBUSY)
                break ;
            SPI2_state = E_SPI2_LCD_WRITE_ENABLE_HIGH;
            SPI2BUF = SPI_buf_LCD[SPI_LCD_index++];
            break ;
        case E_SPI2_LCD_WRITE_ENABLE_HIGH:
            if (SPI2STATbits.SPIBUSY)
                break ;
            if (SPI_LCD_index < SPI_LCD_count)
            {
                SPI2_state = E_SPI2_LCD_WRITE_ENABLE_LOW;
                SPI2BUF = SPI_buf_LCD[SPI_LCD_index] | (LCD_ENABLE_BIT << 8);
                break ;
            }
            else
                SPI2_state = E_SPI2_LCD_RELEASE;
        case E_SPI2_LCD_RELEASE:
            SPI2_state = E_SPI2_DONE;
            SPI2CONbits.MODE16 = 0;
            SS_MCP_LCD = 0x1;
            SPI_LCD_index = 0;
            SPI_LCD_count = 0;
            break;
    }
}

void SPI2_push_LCD_buffer(u16 *buffer, u32 size)
{
    u32  i;

    IEC1bits.SPI2RXIE = 0;
    IEC1bits.SPI2TXIE = 0;
    i = 0;
    while (i < size)
        SPI_buf_LCD[SPI_LCD_count++] = buffer[i++];
    IEC1bits.SPI2RXIE = 1;
    IEC1bits.SPI2TXIE = 1;
}

void    manage_SPI2(void)
{
    if (!SPI2_READY)
        return ;
    if (SPI_LCD_count != 0)
    {
            SPI2_state = E_SPI2_LCD_CONFIG;
            IFS1bits.SPI2RXIF = 1;
            return ;
    }
    else if (SPI_flash_count != 0)
    {
        ;
    }
}