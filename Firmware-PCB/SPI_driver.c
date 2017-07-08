#include <xc.h>
#include <sys/attribs.h>
#include "0x2a002a.h"

extern u8   SPI_eeprom_write_request;
extern u8   SPI_eeprom_read_request;
extern u8   SPI_encoders_dirty;
extern u8   LCD_dirty;
extern u8   LCD_writing;
extern u8   SPI_LCD_clear;

u8          SPI1_state = E_SPI1_DONE;
u8          SPI1_slave = E_SPI1_CS_MCP_ENC;

void SPI1_init(void)
{
    SPI1CON = 0;
    SPI1BUF = 0;
    SPI1BRG = 0; //set baudrate 1Mhz suivant 8 Mhz du pbclk
    SPI1CONbits.CKE = 1;
    SPI1CONbits.CKP = 0; // mode 00 tás vue
    SPI1CONbits.MODE16 = 0;
    SPI1CONbits.MODE32 = 0;
    SPI1CONbits.MSTEN = 1; //activer master mode
    SPI1CONbits.ON = 1; //ON SPI1
}

void    pps_init_SPI1(void)
{
    SPI1_CS0_GPIO = GPIO_OUTPUT;
    CS_MCP_LCD = CS_LINE_UP;
    SPI1_CS1_GPIO = GPIO_OUTPUT;
    CS_MCP_ENCODERS = CS_LINE_UP;
    SPI1_CS2_GPIO = GPIO_OUTPUT;
    CS_EEPROM = CS_LINE_UP;
    SPI1_CS3_GPIO = GPIO_OUTPUT;
    CS_SD = CS_LINE_UP;
    SPI1_CS2_ANALOG = DIGITAL_PIN;
    SPI1_CS3_ANALOG = DIGITAL_PIN;
    SDI1_ANALOG = DIGITAL_PIN;
    PPS_SDI1;
    SDO1_ANALOG = DIGITAL_PIN;
    PPS_SDO1;
}

void    int_init_SPI1(void)
{
    SPI1_INT_FLAGS_CLR_RX;
    SPI1_INT_FLAGS_CLR_TX;
    SPI1_INT_PRIORITIES = 4;
    //PERSISTANT !!!!!!!!!!!!!!!!!!!!!!!!!!!
    SPI1_RECEIVE_ENABLE = INT_DISABLED;
    SPI1_TRANSFER_ENABLE = INT_DISABLED;
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
        case E_SPI1_CS_EEPROM:
            SPI1_eeprom_state_machine();
            break;
    }
}

void    SPI1_manager(void)
{
    if (!SPI1_READY)
        return ;

    if (SPI_encoders_dirty)
    {
        SPI1_slave = E_SPI1_CS_MCP_ENC;
        SPI1_state = E_SPI1_ENC_READ_INT_FLAG;
        SPI1_RECEIVE_FLAG = 1;
        SPI1_RECEIVE_ENABLE = INT_ENABLED;
    }
    else if (LCD_dirty)
    {
        display_current_template();

        SPI1_slave = E_SPI1_CS_MCP_LCD;
        SPI1_state = E_SPI1_LCD_CONFIG;
        SPI1_RECEIVE_FLAG = 1;
        SPI1_RECEIVE_ENABLE = INT_ENABLED;
    }
    else if (SPI_eeprom_write_request)
    {
        SPI1_slave = E_SPI1_CS_EEPROM;
        SPI1_state = E_SPI1_EEPROM_WRITE_ENABLE;
        SPI1_RECEIVE_FLAG = 1;
        SPI1_RECEIVE_ENABLE = INT_ENABLED;
    }
    else if (SPI_eeprom_read_request)
    {
        SPI1_slave = E_SPI1_CS_EEPROM;
        SPI1_state = E_SPI1_EEPROM_READ_ENABLE;
        SPI1_RECEIVE_FLAG = 1;
        SPI1_RECEIVE_ENABLE = INT_ENABLED;
    }
}