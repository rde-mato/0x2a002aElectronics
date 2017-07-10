#include <xc.h>
#include <sys/attribs.h>
#include "0x2a002a.h"

extern u8   SPI1_state;
extern u8   cur_instrument;
extern u8   cur_pattern;
extern u8   active_patterns_array[INSTRUMENTS_COUNT][QTIME_PER_PATTERN][NOTES_PER_QTIME][ATTRIBUTES_PER_NOTE];
extern u8   cur_active_pattern[QTIME_PER_PATTERN][NOTES_PER_QTIME][ATTRIBUTES_PER_NOTE];
extern u8   active_instrument[PATTERNS_PER_INSTRUMENT][QTIME_PER_PATTERN][NOTES_PER_QTIME][ATTRIBUTES_PER_NOTE];

u8          SPI_eeprom_write_request = 0;
u8          SPI_eeprom_read_request = 0;
u16         eeprom_address;
u16         eeprom_cur_address;
u32         eeprom_read_size;
u32         eeprom_read_index = 0;
u8          eeprom_write_buf[QTIME_PER_PATTERN * NOTES_PER_QTIME * ATTRIBUTES_PER_NOTE];
u32         eeprom_write_size;
u32         eeprom_write_index = 0;
u8          eeprom_buf_size;
u8          eeprom_buf[128];

const   size_t pattern_size = QTIME_PER_PATTERN * NOTES_PER_QTIME * ATTRIBUTES_PER_NOTE;
const   size_t instrument_size = INSTRUMENTS_COUNT * QTIME_PER_PATTERN * NOTES_PER_QTIME * ATTRIBUTES_PER_NOTE;

void    active_patterns_init(void)
{
    u8  i = 0;
    u8  qt;
    u8  n;
    u8  read;
    u16 address;

    while (i < INSTRUMENTS_COUNT)
    {
        CS_EEPROM = CS_LINE_DOWN;
        SPI1BUF = E_EEPROM_READ;
        while (SPI1STATbits.SPIBUSY);
        read = SPI1BUF;

        address = i * instrument_size;
        SPI1BUF = (u8)(address >> 8);
        while (SPI1STATbits.SPIBUSY);
        read = SPI1BUF;
        SPI1BUF = (u8)(address);
        while (SPI1STATbits.SPIBUSY);
        read = SPI1BUF;

        qt = 0;
        while (qt < QTIME_PER_PATTERN)
        {
            n = 0;
            while (n < NOTES_PER_QTIME)
            {
                SPI1BUF = 0xFF;
                while (SPI1STATbits.SPIBUSY);
                active_patterns_array[i][qt][n][ATTRIBUTES_PER_NOTE] = SPI1BUF;
                ++n;
            }
            ++qt;
        }
        ++i;
        CS_EEPROM = CS_LINE_UP;
    }
    update_active_pattern();
}

void    update_active_pattern(void)
{
    memcpy(cur_active_pattern, active_patterns_array[cur_instrument][cur_pattern], pattern_size);
}

void    save_cur_pattern_to_eeprom(void)
{
//    EEPROM_requested_instrument = instrument;
//    EEPROM_requested_pattern = pattern;
    memcpy(active_instrument[cur_pattern], cur_active_pattern, pattern_size);
    memcpy(eeprom_write_buf, cur_active_pattern, pattern_size);
    eeprom_address = (PATTERNS_PER_INSTRUMENT * cur_instrument + cur_pattern) * pattern_size;
    eeprom_write_size = pattern_size;
    eeprom_write_index = 0;
    SPI_eeprom_write_request = 1;
}

void    load_cur_instrument_from_eeprom(void)
{
    eeprom_address = cur_instrument * instrument_size;
    SPI_eeprom_read_request = 1;
}

void    active_instrument_init(void)
{
    load_cur_instrument_from_eeprom();
}

void	eeprom_state_machine_write(void)
{
    u8  read8;

    switch (SPI1_state)
    {
        case E_SPI1_EEPROM_WRITE_INSTRUCTION:
            read8 = SPI1BUF;
            eeprom_cur_address = eeprom_address;
            CS_EEPROM = CS_LINE_DOWN;
            SPI1_state = E_SPI1_EEPROM_WRITE_ADDRESS_FIRST_BYTE;
            SPI1BUF = E_EEPROM_WRITE;
            SPI1_TRANSMIT_ENABLE = INT_ENABLED;
            break ;

        case E_SPI1_EEPROM_WRITE_ADDRESS_FIRST_BYTE:
            read8 = SPI1BUF;
            SPI1_state = E_SPI1_EEPROM_WRITE_ADDRESS_SECOND_BYTE;
            SPI1BUF = (u8)(eeprom_cur_address >> 8);
            SPI1_TRANSMIT_ENABLE = INT_ENABLED;
            break ;

        case E_SPI1_EEPROM_WRITE_ADDRESS_SECOND_BYTE:
            read8 = SPI1BUF;
            SPI1_state = E_SPI1_EEPROM_WRITE_DATA;
            SPI1BUF = (u8)(eeprom_cur_address);
            SPI1_TRANSMIT_ENABLE = INT_ENABLED;
            break ;

        case E_SPI1_EEPROM_WRITE_DATA:
            read8 = SPI1BUF;
            if (eeprom_write_index++ >= eeprom_write_size)
                SPI1_state = E_SPI1_EEPROM_WRITE_DONE;
            else
            {
                if ((++eeprom_cur_address & 127) == 0)
                {
                    CS_EEPROM = CS_LINE_UP;
                    SPI1_state = E_SPI1_EEPROM_WRITE_INSTRUCTION;
                }
                SPI1BUF = eeprom_write_buf[eeprom_write_index];
//                eeprom_cur_address++;
                SPI1_TRANSMIT_ENABLE = INT_ENABLED;
                break ;
            }

        case E_SPI1_EEPROM_WRITE_DONE:
            SPI1_state = E_SPI1_DONE;
            eeprom_write_size = 0;
            eeprom_write_index = 0;
            SPI_eeprom_write_request = 0;
            CS_EEPROM = CS_LINE_UP;
            request_template(TEMPLATE_PATTERN_RECORDED);
            break;
    }
}


void	eeprom_state_machine_read(void)
{
    u8  read8;

    switch (SPI1_state)
    {
        case E_SPI1_EEPROM_READ_INSTRUCTION:
            CS_EEPROM = CS_LINE_DOWN;
            SPI1_state = E_SPI1_EEPROM_READ_ADDRESS_FIRST_BYTE;
            SPI1BUF = E_EEPROM_READ;
            SPI1_TRANSMIT_ENABLE = INT_ENABLED;
            break ;

        case E_SPI1_EEPROM_READ_ADDRESS_FIRST_BYTE:
            read8 = SPI1BUF;
            SPI1_state = E_SPI1_EEPROM_READ_ADDRESS_SECOND_BYTE;
            SPI1BUF = (u8)(eeprom_address >> 8);
            SPI1_TRANSMIT_ENABLE = INT_ENABLED;
            break ;

        case E_SPI1_EEPROM_READ_ADDRESS_SECOND_BYTE:
            read8 = SPI1BUF;
            SPI1_state = E_SPI1_EEPROM_READ_DATA;
            SPI1BUF = (u8)(eeprom_address);
            SPI1_TRANSMIT_ENABLE = INT_ENABLED;
            break ;

        case E_SPI1_EEPROM_READ_DATA:
            ((u8 *)active_instrument)[eeprom_read_index++] = SPI1BUF;
            if (eeprom_read_index >= instrument_size)
                SPI1_state = E_SPI1_EEPROM_READ_DONE;
            else
            {
                SPI1BUF = 0xFF;
                SPI1_RECEIVE_ENABLE = INT_ENABLED;
                break ;
            }
        case E_SPI1_EEPROM_READ_DONE:
            CS_EEPROM = CS_LINE_UP;
            SPI1_state = E_SPI1_DONE;
            eeprom_read_index = 0;
            SPI_eeprom_read_request = 0;
            update_active_pattern();
            break;
    }
}

void SPI1_eeprom_state_machine(void)
{
    if (SPI_eeprom_read_request)
	eeprom_state_machine_read();
    else if (SPI_eeprom_write_request)
	eeprom_state_machine_write();
}

void    eeprom_init(void)
{
    u8 read;

    CS_EEPROM = CS_LINE_DOWN;

    SPI1BUF = E_EEPROM_WREN;
    while (SPI1STATbits.SPIBUSY);
    read = SPI1BUF;

    CS_EEPROM = CS_LINE_UP;
}

void    eeprom_chip_erase(void)
{
    u8  read;

    CS_EEPROM = CS_LINE_DOWN;
    SPI1BUF = E_EEPROM_WREN;
    while (SPI1STATbits.SPIBUSY);
    read = SPI1BUF;
    CS_EEPROM = CS_LINE_UP;


    CS_EEPROM = CS_LINE_DOWN;
    SPI1BUF = E_EEPROM_CE;
    while (SPI1STATbits.SPIBUSY);
    read = SPI1BUF;
    CS_EEPROM = CS_LINE_UP;

}

/*
void eeprom_write(u8 byte)
{
	u16 read;

	CS_EEPROM = 0;

	//write enable
	SPI1BUF = 0x06;
	while (SPI1STATbits.SPIBUSY);
	eeprom_read_byte = SPI1BUF;

	CS_EEPROM = 1;

	//wait for write enable ok
	//read_status_register RDSR : 0x05
	SPI1CONbits.MODE16 = 1;
	SPI1BUF = 0x0500;
	while (SPI1STATbits.SPIBUSY);
	eeprom_read_byte = SPI1BUF;
	SPI1CONbits.MODE16 = 0;


	//write instruction
	CS_EEPROM = 0;

	SPI1BUF = 0x02;
	while (SPI1STATbits.SPIBUSY);
	eeprom_read_byte = SPI1BUF;

	//write address
	SPI1CONbits.MODE16 = 1;
	SPI1BUF = 0x0000;
	while (SPI1STATbits.SPIBUSY);
	read = SPI1BUF;
	SPI1CONbits.MODE16 = 0;

	//write data

	SPI1BUF = byte;
	while (SPI1STATbits.SPIBUSY);
	read = SPI1BUF;

	CS_EEPROM = 1;
}

void eeprom_read(u16 add)
{
	u16 read;

	CS_EEPROM = 0;

	SPI1BUF = 0x03;
	while (SPI1STATbits.SPIBUSY);
	eeprom_read_byte = SPI1BUF;

	SPI1CONbits.MODE16 = 1;
	SPI1BUF = add;
	while (SPI1STATbits.SPIBUSY);
	read = SPI1BUF;
	SPI1CONbits.MODE16 = 0;

	SPI1BUF = 0x00;
	while (SPI1STATbits.SPIBUSY);
	eeprom_read_byte = SPI1BUF;

	CS_EEPROM = 1;
}
*/

//void    eeprom_write(u8 *src, u8 size, u16 address)
//{
//    u8 i;
//
//    i = 0;
//    while (i < size)
//    {
//        eeprom_buf[i++] = *src++;
//    }
//    eeprom_buf_size = size;
//    eeprom_address = address;
//    SPI_eeprom_write_request = 1;
//}
//
//void    eeprom_buf_bzero(void)
//{
//    u8  size;
//
//    size = 128;
//    while (size)
//    {
//        eeprom_buf[--size] = 0;
//    }
//}
//
//void    eeprom_read(u8 size, u16 address)
//{
//    eeprom_buf_bzero();
//    eeprom_address = address;
//    eeprom_buf_size = size;
//    SPI_eeprom_read_request = 1;
//}
//
//void    eeprom_read_buf(u8 *dest, u8 size)
//{
//    u8  i;
//
//    i = 0;
//    while (i < size)
//    {
//        dest[i] = eeprom_buf[i];
//        ++i;
//    }
//}


//void eeprom_read(u16 add)
//{
//	u16 read;
//
//	CS_EEPROM = 0;
//
//	SPI1BUF = 0x03;
//	while (SPI1STATbits.SPIBUSY);
//	eeprom_read_byte = SPI1BUF;
//
//	SPI1CONbits.MODE16 = 1;
//	SPI1BUF = add;
//	while (SPI1STATbits.SPIBUSY);
//	read = SPI1BUF;
//	SPI1CONbits.MODE16 = 0;
//
//	SPI1BUF = 0x00;
//	while (SPI1STATbits.SPIBUSY);
//	eeprom_read_byte = SPI1BUF;
//
//	CS_EEPROM = 1;
//}