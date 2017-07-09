#include <xc.h>
#include <sys/attribs.h>
#include "0x2a002a.h"

extern u8   SPI1_state;

u8          SPI_encoders_dirty = 0;

void    pps_init_encoders(void)
{
    // Main encoder A
    COD_A_ANALOG = DIGITAL_PIN;
    COD_A_GPIO = GPIO_INPUT;
    PPS_COD_A;

    // Main encoder B
    COD_B_ANALOG = DIGITAL_PIN;
    COD_B_GPIO = GPIO_INPUT;
    PPS_COD_B;

    //MCP encoders
    PPS_MCP_ENC_A;
    MCP_ENC_A_ANALOG = DIGITAL_PIN;
    COD_MCP_A_GPIO = GPIO_INPUT;
    PPS_MCP_ENC_B;
    MCP_ENC_B_ANALOG = DIGITAL_PIN;
    COD_MCP_B_GPIO = GPIO_INPUT;
}

void    int_init_main_encoder(void)
{
    COD_A_INT_POLARITY = RISING_EDGE;
    COD_A_INT_FLAG_CLR;
    COD_A_INT_PRIORITY = 5;
    COD_A_INT_ENABLE = INT_ENABLED;
//    COD_B_INT_POLARITY = RISING_EDGE;
//    COD_B_INT_FLAG_CLR;
//    COD_B_INT_PRIORITY = 2;
//    COD_B_INT_ENABLE = INT_ENABLED;
}

void    int_init_MCP_encoders(void)
{
//    COD_MCP_A_INT_POLARITY = FALLING_EDGE;
//    COD_MCP_A_INT_FLAG_CLR;
//    COD_MCP_A_INT_PRIORITY = 3;
//    COD_MCP_A_INT_ENABLE = INT_ENABLED;
    COD_MCP_B_INT_POLARITY = FALLING_EDGE;
    COD_MCP_B_INT_FLAG_CLR;
    COD_MCP_B_INT_PRIORITY = 5;
    COD_MCP_B_INT_ENABLE = INT_ENABLED;
}

void MCP_ENCODERS_init_blocking(void)
{
    u16 read;

    SPI1CONbits.MODE16 = 1;

    //read GPIO pour clear flags
    CS_MCP_ENCODERS = 0x0;
    SPI1BUF = 0x4110;
    while (SPI1STATbits.SPIBUSY) ;
    read = SPI1BUF;
    SPI1BUF = 0x0000;
    while (SPI1STATbits.SPIBUSY) ;
    read = SPI1BUF;
    CS_MCP_ENCODERS = 0x1;


    //activation des interrupts sur le port A
    CS_MCP_ENCODERS = 0x0;
    SPI1BUF = 0x4004;
    while (SPI1STATbits.SPIBUSY) ;
    read = SPI1BUF;
    SPI1BUF = 0xFFFF;
    while (SPI1STATbits.SPIBUSY) ;
    read = SPI1BUF;
    CS_MCP_ENCODERS = 0x1;

    SPI1CONbits.MODE16 = 0;
}

void __ISR(_EXTERNAL_2_VECTOR, IPL5AUTO) Main_encoder_A_Handler(void) {
    if (PORTBbits.RB3)
        event_handler(E_ENCODER_TURNED_LEFT, E_SOURCE_ENCODER_MAIN);
    else
        event_handler(E_ENCODER_TURNED_RIGHT, E_SOURCE_ENCODER_MAIN);
    COD_A_INT_FLAG_CLR;
}

/* void __ISR(_EXTERNAL_1_VECTOR, IPL3AUTO) MCP_encoders_port_A_Handler(void) {
    COD_MCP_A_INT_FLAG_CLR;
    SPI_encoders_dirty = 1;
} XD */

void __ISR(_EXTERNAL_3_VECTOR, IPL5AUTO) MCP_encoders_port_B_Handler(void) {
    COD_MCP_B_INT_FLAG_CLR;
    SPI_encoders_dirty = 1;
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
            SPI1_TRANSMIT_ENABLE = INT_ENABLED;
            break;
        case E_SPI1_ENC_READ_INT_CAP:
            COD_MCP_B_INT_ENABLE = INT_DISABLED;
            SPI_encoders_dirty = 0;
            COD_MCP_B_INT_ENABLE = INT_ENABLED;
            read32 = SPI1BUF;
            flags_A = read32 >> 8;
            CS_MCP_ENCODERS = 1;
            SPI1_state = E_SPI1_ENC_RELEASE;
            CS_MCP_ENCODERS = 0;
            SPI1BUF = MCP_ENC_READ_INT_CAP;
            SPI1_RECEIVE_ENABLE = INT_ENABLED;
            SPI1_TRANSMIT_ENABLE = INT_ENABLED;
            break;
        case E_SPI1_ENC_RELEASE:
            read32 = SPI1BUF;
            IEC0bits.INT3IE = 1;
            intcap_A = (u8)(read32 >> 8);
            intcap_B = (u8)read32;
            CS_MCP_ENCODERS = 1;
            SPI1_state = E_SPI1_DONE;
            while (i < 8)
            {
                if (flags_A & (1 << i))
                    event_handler((intcap_A & (1 << i)) != (intcap_B & (1 << i)) ? E_ENCODER_TURNED_LEFT : E_ENCODER_TURNED_RIGHT, i + E_SOURCE_ENCODER_0);
                ++i;
            }
            SPI1CONbits.MODE32 = 0;
            break;
    }
}