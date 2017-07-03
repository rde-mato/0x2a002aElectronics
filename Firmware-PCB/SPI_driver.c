#include <xc.h>
#include <sys/attribs.h>
#include "0x2a002a.h"

#define SLAVE_COUNT 4
#define LCD_BUF_MAX 30 //128 * ( 8 + 2 )
#define FLASH_BUF_MAX 30                        // calculs a faire


u8  SPI1_state = E_SPI1_DONE;
u8  SPI1_slave = E_SPI1_CS_MCP_ENC;

u16	SPI_buf_LCD[LCD_BUF_MAX] = { 0 };
u32	SPI_LCD_index = 0;
u32	SPI_LCD_count = 0;

u16	SPI_buf_FLASH[FLASH_BUF_MAX] = { 0 };
u32     SPI_flash_index = 0;
u32     SPI_flash_count = 0;

u8      SPI_encoders_dirty = 0;

void    SPI1_LCD_state_machine(void)
{
    u32 clear;
    switch (SPI1_state)
    {
        case E_SPI1_LCD_CONFIG:
            SPI1CONbits.MODE16 = 1;
            CS_MCP_LCD = 0x0;
            SPI1_state = E_SPI1_LCD_WRITE_ENABLE_HIGH;
            SPI1BUF = LCD_PORTS_ADDRESS;
            SPI1_RECEIVE_ENABLE = INT_ENABLED;
            SPI1_TRANSFER_ENABLE = INT_ENABLED;
            break;
        case E_SPI1_LCD_WRITE_ENABLE_LOW:
            if (SPI1STATbits.SPIBUSY)
                break ;
            clear = SPI1BUF;
            SPI1_state = E_SPI1_LCD_WRITE_ENABLE_HIGH;
            SPI1BUF = SPI_buf_LCD[SPI_LCD_index++];
            SPI1_RECEIVE_ENABLE = INT_ENABLED;
            SPI1_TRANSFER_ENABLE = INT_ENABLED;
            break ;
        case E_SPI1_LCD_WRITE_ENABLE_HIGH:
            if (SPI1STATbits.SPIBUSY)
                break ;
            if (SPI_LCD_index < SPI_LCD_count)
            {
                clear = SPI1BUF;
                SPI1_state = E_SPI1_LCD_WRITE_ENABLE_LOW;
                SPI1BUF = SPI_buf_LCD[SPI_LCD_index] | (LCD_ENABLE_BIT << 8);
                SPI1_RECEIVE_ENABLE = INT_ENABLED;
                SPI1_TRANSFER_ENABLE = INT_ENABLED;
                break ;
            }
            else
                SPI1_state = E_SPI1_LCD_RELEASE;
        case E_SPI1_LCD_RELEASE:
            clear = SPI1BUF;
            SPI1_state = E_SPI1_DONE;
            SPI1CONbits.MODE16 = 0;
            CS_MCP_LCD = 0x1;
            SPI_LCD_index = 0;
            SPI_LCD_count = 0;
            break;
    }
}

void    SPI1_ENC_state_machine(void)
{
    u32 read32;
    static u8 flags_A;
    u8 i = 0;
    u8      intcap_A;
    u8      intcap_B;

    switch (SPI1_state)
    {
        case E_SPI1_ENC_READ_INT_FLAG:
            SPI1CONbits.MODE32 = 1;
            CS_MCP_ENCODERS = 0x0;
            SPI1_state = E_SPI1_ENC_READ_INT_CAP;
            SPI1BUF = MCP_ENC_READ_INT_FLAG;
            SPI1_RECEIVE_ENABLE = INT_ENABLED;
            SPI1_TRANSFER_ENABLE = INT_ENABLED;
            break;
        case E_SPI1_ENC_READ_INT_CAP:
            read32 = SPI1BUF;
            flags_A = read32 >> 8;
            CS_MCP_ENCODERS = 1;
            SPI1_state = E_SPI1_ENC_RELEASE;
            CS_MCP_ENCODERS = 0;
            SPI1BUF = MCP_ENC_READ_INT_CAP;
            SPI1_RECEIVE_ENABLE = INT_ENABLED;
            SPI1_TRANSFER_ENABLE = INT_ENABLED;
            break;
        case E_SPI1_ENC_RELEASE:
            read32 = SPI1BUF;
            intcap_A = (u8)(read32 >> 8);
            intcap_B = (u8)read32;
            CS_MCP_ENCODERS = 1;
            SPI1_state = E_SPI1_DONE;
            while (i < 8)
            {
                if (flags_A & (1 << i))
                    event_handler((intcap_A & (1 << i)) != (intcap_B & (1 << i)) ? E_ENCODER_TURNED_RIGHT : E_ENCODER_TURNED_LEFT, i);
                ++i;
            }
            SPI_encoders_dirty = 0;
            SPI1CONbits.MODE32 = 0;
            break;
    }
}

void __ISR(_SPI_1_VECTOR, IPL4AUTO) SPI1Handler(void)           // a voir apres comment faire plusieurs state machines en fonction du slave
{
    SPI1_RECEIVE_ENABLE = INT_DISABLED;
    SPI1_TRANSFER_ENABLE = INT_DISABLED;

    SPI1_INT_FLAGS_CLR_RX;
    SPI1_INT_FLAGS_CLR_TX;

    switch (SPI1_slave)
    {
        case E_SPI1_CS_MCP_LCD:
            SPI1_LCD_state_machine();
            break;
        case E_SPI1_CS_MCP_ENC:
            SPI1_ENC_state_machine();
            break;
    



//        case E_SPI1_FLASH_ERASE_CONFIG:
//            CS_FLASH = 0x0;
//            SPI1_state = xxx;
//            break;
//
//        case E_SPI1_FLASH_WRITE_CONFIG:
//
//
//        case E_SPI1_FLASH_READ_CONFIG:
//
//
//        case E_SPI1_FLASH_WRITE:
//        case E_SPI1_FLASH_READ:
//        case E_SPI1_FLASH_ERASE:
//        case E_SPI1_FLASH_BUSY:

    }
}

void SPI1_push_LCD_buffer(u16 *buffer, u32 size)
{
    u32  i;

    IEC1bits.SPI1RXIE = 0;
    IEC1bits.SPI1TXIE = 0;
    i = 0;
    while (i < size)
        SPI_buf_LCD[SPI_LCD_count++] = buffer[i++];
    IEC1bits.SPI1RXIE = 1;
    IEC1bits.SPI1TXIE = 1;
}

void    manage_SPI1(void)
{
    if (!SPI1_READY)
        return ;
    if (SPI_LCD_count != 0)
    {
        SPI1_slave = E_SPI1_CS_MCP_LCD;
        SPI1_state = E_SPI1_LCD_CONFIG;
        IFS1bits.SPI1RXIF = 1;
//        IFS1bits.SPI1TXIF = 1;
        SPI1_RECEIVE_ENABLE = INT_ENABLED;
//        SPI1_TRANSFER_ENABLE = INT_ENABLED;
        return ;
    }
    else if (SPI_encoders_dirty)
    {
        SPI1_slave = E_SPI1_CS_MCP_ENC;
        SPI1_state = E_SPI1_ENC_READ_INT_FLAG;
        IFS1bits.SPI1RXIF = 1;
        SPI1_RECEIVE_ENABLE = INT_ENABLED;
        return ;
    }
}