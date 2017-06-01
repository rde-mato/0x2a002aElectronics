#include <xc.h>
#include <sys/attribs.h>
#include "0x2a002a.h"

#define SPI_MODE_32 0
#define SPI_MODE_24 1

u8  SPI2_state = E_SPI2_DONE;
u8  SPI2_transmit = SPI_MODE_32;

u32 transmit32;
u8  transmit24[3];
u8  transmit_index = 0;

u32 read32;
u8  read8;


void __ISR(_SPI_2_VECTOR, IPL4AUTO) SPI2Handler(void)
{
    IFS1bits.SPI2RXIF = 0;
    IFS1bits.SPI2TXIF = 0;
    SPI2_state_machine();
}

void    SPI2_state_machine(void)
{
    switch (SPI2_state)
    {
        case E_SPI2_CONFIG:
            if (SPI2_transmit == SPI_MODE_32)
            {
                SPI2CONbits.MODE32 = 1;
                SPI2_state = E_SPI2_WRITE_32;
            }
            else
            {
                SPI2CONbits.MODE32 = 0;
                SPI2_state = E_SPI2_WRITE_8;
            }
            LATGbits.LATG9 = 0x0;
        case E_SPI2_WRITE_32:
            SPI2_state = E_SPI2_READ_32;
            SPI2BUF = transmit32;
            break;
        case E_SPI2_WRITE_8:
            SPI2BUF = transmit24[transmit_index++];
            SPI2_state = E_SPI2_READ_8;
            break;
        case E_SPI2_READ_32:
            SPI2_state = E_SPI2_RELEASE;
            read32 = SPI2BUF;
        case E_SPI2_READ_8:
            read8 = SPI2BUF;
            if (transmit_index < 3)
            {
                SPI2_state = E_SPI2_WRITE_8;
                break;
            }
            else
                SPI2_state = E_SPI2_RELEASE;
        case E_SPI2_RELEASE:
            SPI2_state = E_SPI2_DONE;
            LATGbits.LATG9 = 0x1;
            break;
    }
}

u32 SPI2_transmit32bits(u32 msg) //rajouter en argument le SS du chip quand on aura plusieur
{
    transmit32 = msg;
    SPI2_state_machine();
}

u8 SPI2_transmit24bits(u8 part1, u8 part2, u8 part3) //rajouter en argument le SS du chip quand on aura plusieur
{
    transmit24[0] = part1;
    transmit24[1] = part2;
    transmit24[2] = part3;
    SPI2_state_machine();
}