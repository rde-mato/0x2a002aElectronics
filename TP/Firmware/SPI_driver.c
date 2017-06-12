#include <xc.h>
#include <sys/attribs.h>
#include "0x2a002a.h"

#define SLAVE_COUNT 4
#define LCD_BUF_MAX 30


u8  SPI2_state = E_SPI2_DONE;
u8  SPI2_slave = E_SPI2_SS_MCP_LCD;

u8	SPI_buf_LCD[LCD_BUF_MAX] = { 0 };   /// !!!!!!!!!!!!! A CHANGER ABSOLUMENT !!!!
u8      *SPI_slave_buffers[SLAVE_COUNT] = {SPI_buf_LCD, 0, 0, 0};
u8      SPI_slave_sizes[SLAVE_COUNT] = {0, 0, 0, 0};
u8      SPI_slave_indexes[SLAVE_COUNT] = {0, 0, 0, 0};

u8  current_slave = E_SPI2_SS_MCP_LCD;
u32 read32;


void __ISR(_SPI_2_VECTOR, IPL4AUTO) SPI2Handler(void)           // a voir apres comment faire plusieurs state machines en fonction du slave
{
    u8  read8;
    u8  write8;

//    SS_MCP_LCD = 0x0;
//    while (SPI_slave_indexes[current_slave] < SPI_slave_sizes[current_slave])
//    {
//        SPI2BUF = SPI_slave_buffers[current_slave][(SPI_slave_indexes[current_slave])++];
//        while (!SPI2STATbits.SPIRBF) ;
//        read8 = SPI2BUF;
//    }
//    SPI_slave_sizes[current_slave] = 0;
//    SPI_slave_indexes[current_slave] = 0;
//    SS_MCP_LCD = 0x1;
//    SPI2_state = E_SPI2_DONE;

    IFS1bits.SPI2RXIF = 0;
    IFS1bits.SPI2TXIF = 0;
    


    SS_MCP_LCD = 0x0;
    switch (SPI2_state)
    {
        case E_SPI2_READ_BEFORE_WRITE:
            read8 = SPI2BUF;
            SPI2_state = E_SPI2_WRITE;
        case E_SPI2_WRITE:
            if (SPI2STATbits.SPIBUSY)
                break ;
            if (SPI_slave_indexes[current_slave] < SPI_slave_sizes[current_slave])
            {
                write8 = SPI_slave_buffers[current_slave][(SPI_slave_indexes[current_slave])++];
                SPI2_state = E_SPI2_READ_BEFORE_WRITE;
                SPI2BUF = write8;
                break ;
            }
            else
                SPI2_state = E_SPI2_RELEASE;
        case E_SPI2_RELEASE:
            SPI2_state = E_SPI2_DONE;
            SS_MCP_LCD = 0x1;
            SPI_slave_sizes[current_slave] = 0;
            SPI_slave_indexes[current_slave] = 0;
            break;
    }
}

//void    SPI2_push(u8 slave, u8 data)
//{
//    SPI_slave_buffers[slave][SPI_slave_sizes[slave]++] = data;
//}



//u32 SPI2_transmit32bits(u32 msg) //rajouter en argument le SS du chip quand on aura plusieur
//{
//    transmit32 = msg;
//	IFS1bits.SPI2RXIF = 1;
//}


u8 SPI2_push_buffer(u8 slave, u8 *buffer, u8 size) //rajouter en argument le SS du chip quand on aura plusieur
{
    u8  i;

    //SPI2_writing = 1;
    i = 0;
    while (i < size)
        SPI_slave_buffers[slave][(SPI_slave_sizes[slave])++] = buffer[i++];
    //SPI2_writing = 0;
}



//u8 SPI2_transmit24bits(u8 slave, u8 part1, u8 part2, u8 part3) //rajouter en argument le SS du chip quand on aura plusieur
//{
////    transmit24[0] = part1;
////    transmit24[1] = part2;
////    transmit24[2] = part3;
//    SPI2_push(slave, part1);
//    SPI2_push(slave, part2);
//    SPI2_push(slave, part3);
//    SPI2_state = E_SPI2_CONFIG;
//    IFS1bits.SPI2RXIF = 1;
//}

void    manage_SPI2(void)
{
    u8  i = 0;

    if (!SPI2_READY)
        return ;
    while (i < SLAVE_COUNT)
    {
        if (SPI_slave_sizes[i] != 0)
        {
            current_slave = i;
            SPI2_state = E_SPI2_WRITE;
            IFS1bits.SPI2RXIF = 1;
            return ;
        }
        ++i;
    }
}