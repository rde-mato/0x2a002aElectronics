#include <xc.h>
#include <sys/attribs.h>
#include "0x2a002a.h"

extern u8   SPI1_state;

u8          SPI_eeprom_write_request = 0;
u8          SPI_eeprom_read_request = 0;
u8          EEPROM_requested_instrument = 0;

static u16  eeprom_address;
static u8   eeprom_buf_size;
static u8   eeprom_buf[128]; //size of a page
//u8          instrument_buffer[PATTERNS_PER_INSTRUMENT * QTIME_PER_PATTERN * NOTES_PER_QTIME * ATTRIBUTES_PER_NOTE];

extern u8          active_patterns[INSTRUMENTS_COUNT][QTIME_PER_PATTERN][NOTES_PER_QTIME][ATTRIBUTES_PER_NOTE];
extern u8          active_instrument[PATTERNS_PER_INSTRUMENT][QTIME_PER_PATTERN][NOTES_PER_QTIME][ATTRIBUTES_PER_NOTE];

const   size_t pattern_size = QTIME_PER_PATTERN * NOTES_PER_QTIME * ATTRIBUTES_PER_NOTE;
const   size_t instrument_size = INSTRUMENTS_COUNT * QTIME_PER_PATTERN * NOTES_PER_QTIME * ATTRIBUTES_PER_NOTE;

void    eeprom_write(u8 *src, u8 size, u16 address)
{
    u8 i;
    
    i = 0;
    while (i < size)
    {
        eeprom_buf[i++] = *src++;
    }
    eeprom_buf_size = size;
    eeprom_address = address;
    SPI_eeprom_write_request = 1;
}

void    eeprom_buf_bzero(void)
{
    u8  size;
    
    size = 128;
    while (size)
    {
        eeprom_buf[--size] = 0;
    }
}

void    eeprom_read(u8 size, u16 address)
{
    eeprom_buf_bzero();
    eeprom_address = address;
    eeprom_buf_size = size;
    SPI_eeprom_read_request = 1;
}

void    eeprom_read_buf(u8 *dest, u8 size)
{
    u8  i;
    
    i = 0;
    while (i < size)
    {
        dest[i] = eeprom_buf[i];
        ++i;
    }
}

void    save_pattern_to_eeprom(u8 instrument, u8 pattern)
{
//    u32 i;
//    u8  qt;
//    u8  n;
//
//    i = 0;
//    qt = 0;
//    n = 0;
//    while (qt < QTIME_PER_PATTERN)
//    {
//        while (n < NOTES_PER_QTIME)
//        {
//            eeprom_buf[] = ;
//        }
//    }
//    eeprom_address = (PATTERNS_PER_INSTRUMENT * instrument + pattern) * pattern_size;
}

void	eeprom_state_machine_write(void)
{
    u16		eeprom_read_byte;
    static u8	eeprom_buf_i = 0;

    switch (SPI1_state)
    {
        case E_SPI1_EEPROM_WRITE_ENABLE:
            eeprom_buf_i = 0;
            CS_EEPROM = 0;
            SPI1BUF = EEPROM_WREN; // Write enable latch
            SPI1_state = E_SPI1_EEPROM_WRITE_ENABLE_OK;
            SPI1_RECEIVE_ENABLE = INT_ENABLED;
            SPI1_TRANSFER_ENABLE = INT_ENABLED;
            break;
        case E_SPI1_EEPROM_WRITE_ENABLE_OK:
            if (SPI1STATbits.SPIBUSY)
                break;
            eeprom_read_byte = SPI1BUF;
            CS_EEPROM = 1;
            SPI1_state = E_SPI1_EEPROM_WAIT_WRITE_STAT_OK;
            SPI1CONbits.MODE16 = 1;
            SPI1_RECEIVE_ENABLE = INT_ENABLED;
            SPI1_TRANSFER_ENABLE = INT_ENABLED;
            break;
        case E_SPI1_EEPROM_WAIT_WRITE_STAT_OK:
            CS_EEPROM = 0;
            SPI1BUF = (0xffff & EEPROM_RDSR) << 8;// read status byte + 1 byte to read back
            SPI1_state = E_SPI1_EEPROM_STILL_WAIT_WRITE_STAT_OK;
            SPI1_RECEIVE_ENABLE = INT_ENABLED;
            SPI1_TRANSFER_ENABLE = INT_ENABLED;
            break;
        case E_SPI1_EEPROM_STILL_WAIT_WRITE_STAT_OK:
            eeprom_read_byte = SPI1BUF;
            if (eeprom_read_byte & EEPROM_RDSR_WEL) // Check in STATUS register if Write Enable Latch is ok
                SPI1_state = E_SPI1_EEPROM_WRITE_MODE;
            else
                SPI1_state = E_SPI1_EEPROM_WAIT_WRITE_STAT_OK;
            CS_EEPROM = 1;
            SPI1_RECEIVE_ENABLE = INT_ENABLED;
            SPI1_TRANSFER_ENABLE = INT_ENABLED;
            break;
        case E_SPI1_EEPROM_WRITE_MODE:
            if (SPI1STATbits.SPIBUSY)
                break;
            SPI1CONbits.MODE16 = 0;
            CS_EEPROM = 0;
            SPI1BUF = EEPROM_WRITE; // Write instruction
            SPI1_state = E_SPI1_EEPROM_WRITE_ADDRESS;
            SPI1_RECEIVE_ENABLE = INT_ENABLED;
            SPI1_TRANSFER_ENABLE = INT_ENABLED;
            break;
        case E_SPI1_EEPROM_WRITE_ADDRESS:
            if (SPI1STATbits.SPIBUSY)
                break;
            eeprom_read_byte = SPI1BUF;
            SPI1CONbits.MODE16 = 1;
            SPI1BUF = eeprom_address; // Send 16 bit address
            SPI1_state = E_SPI1_EEPROM_WRITE_DATA;
            SPI1_RECEIVE_ENABLE = INT_ENABLED;
            SPI1_TRANSFER_ENABLE = INT_ENABLED;
            break;
        case E_SPI1_EEPROM_WRITE_DATA:
            if (SPI1STATbits.SPIBUSY)
                break;
            eeprom_read_byte = SPI1BUF;
            SPI1CONbits.MODE16 = 0;
            SPI1BUF = eeprom_buf[eeprom_buf_i++];
            if (eeprom_buf_i == eeprom_buf_size)
                SPI1_state = E_SPI1_EEPROM_WRITE_END;
            if ((++eeprom_address & 0xff) == 0)
                SPI1_state = E_SPI1_EEPROM_WRITE_NEXT_PAGE;
            SPI1_RECEIVE_ENABLE = INT_ENABLED;
            SPI1_TRANSFER_ENABLE = INT_ENABLED;
            break;
        case E_SPI1_EEPROM_WRITE_NEXT_PAGE:
            if (SPI1STATbits.SPIBUSY)
                break;
            eeprom_read_byte = SPI1BUF;
            CS_EEPROM = 1;
            SPI1_state = E_SPI1_EEPROM_WRITE_MODE;
            SPI1_RECEIVE_ENABLE = INT_ENABLED;
            SPI1_TRANSFER_ENABLE = INT_ENABLED;
            break;
        case E_SPI1_EEPROM_WRITE_END:
            if (SPI1STATbits.SPIBUSY)
                break;
            eeprom_read_byte = SPI1BUF;
            CS_EEPROM = 1;
            SPI1_state = E_SPI1_EEPROM_WRITE_DISABLE;
            SPI1_RECEIVE_ENABLE = INT_ENABLED;
            SPI1_TRANSFER_ENABLE = INT_ENABLED;
            break;
        case E_SPI1_EEPROM_WRITE_DISABLE:
            CS_EEPROM = 0;
            SPI1_state = E_SPI1_EEPROM_WRITE_DISABLE_OK;
            SPI1BUF = EEPROM_WRDI;
            SPI1_RECEIVE_ENABLE = INT_ENABLED;
            SPI1_TRANSFER_ENABLE = INT_ENABLED;
            break;
        case E_SPI1_EEPROM_WRITE_DISABLE_OK:
            if (SPI1STATbits.SPIBUSY)
                break;
            eeprom_read_byte = SPI1BUF;
            CS_EEPROM = 1;
            SPI1_state = E_SPI1_EEPROM_WAIT_WRDI_OK;
            SPI1CONbits.MODE16 = 1;
            SPI1_RECEIVE_ENABLE = INT_ENABLED;
            SPI1_TRANSFER_ENABLE = INT_ENABLED;
            break;
        case E_SPI1_EEPROM_WAIT_WRDI_OK:
            CS_EEPROM = 0;
            SPI1BUF = (0xffff & EEPROM_RDSR) << 8;// read status byte + 1 byte to read back
            SPI1_state = E_SPI1_EEPROM_STILL_WAIT_WRDI_OK;
            SPI1_RECEIVE_ENABLE = INT_ENABLED;
            SPI1_TRANSFER_ENABLE = INT_ENABLED;
            break;
        case E_SPI1_EEPROM_STILL_WAIT_WRDI_OK:
            eeprom_read_byte = SPI1BUF;
            if (!(eeprom_read_byte & EEPROM_RDSR_WEL)) // Check in STATUS register if Write Enable Latch is off
                SPI1_state = E_SPI1_EEPROM_WRITE_DONE;
            else
                SPI1_state = E_SPI1_EEPROM_WAIT_WRDI_OK;
            CS_EEPROM = 1;
            SPI1_RECEIVE_ENABLE = INT_ENABLED;
            SPI1_TRANSFER_ENABLE = INT_ENABLED;
            break;
        case E_SPI1_EEPROM_WRITE_DONE:
            SPI1CONbits.MODE16 = 0;
            SPI1_state = E_SPI1_DONE;
            eeprom_buf_i = 0;
            SPI_eeprom_write_request = 0;
            break;
    }
}


void    eeprom_state_machine_read(void)
{
    u16         eeprom_read_byte;
    static u8	eeprom_buf_i = 0;

    switch (SPI1_state)
    {
        case E_SPI1_EEPROM_READ_ENABLE:
            eeprom_buf_i = 0;
            SPI1CONbits.MODE16 = 1;
            CS_EEPROM = 0;
            SPI1BUF = (0xffff & EEPROM_RDSR) << 8; // Read status byte instruction + 1 byte to read back
            SPI1_state = E_SPI1_EEPROM_READ_ENABLE_OK;
            SPI1_RECEIVE_ENABLE = INT_ENABLED;
            SPI1_TRANSFER_ENABLE = INT_ENABLED;
            break;
        case E_SPI1_EEPROM_READ_ENABLE_OK:
            eeprom_read_byte = SPI1BUF;
            if (eeprom_read_byte & EEPROM_RDSR_WIP) // Check in STATUS if Write In Progress is ON
                SPI1_state = E_SPI1_EEPROM_READ_ENABLE;
            else
                SPI1_state = E_SPI1_EEPROM_READ_MODE;
            CS_EEPROM = 1;
            SPI1_RECEIVE_ENABLE = INT_ENABLED;
            SPI1_TRANSFER_ENABLE = INT_ENABLED;
            break;
        case E_SPI1_EEPROM_READ_MODE:
            CS_EEPROM = 0;
            SPI1CONbits.MODE16 = 0;
            SPI1BUF = EEPROM_READ; //READ instruction
            SPI1_state = E_SPI1_EEPROM_READ_ADDRESS;
            SPI1_RECEIVE_ENABLE = INT_ENABLED;
            SPI1_TRANSFER_ENABLE = INT_ENABLED;
            break;
        case E_SPI1_EEPROM_READ_ADDRESS:
            if (SPI1STATbits.SPIBUSY)
                break;
            eeprom_read_byte = SPI1BUF;
            SPI1CONbits.MODE16 = 1;
            SPI1BUF = eeprom_address;
            SPI1_state = E_SPI1_EEPROM_WAIT_READ_STAT_OK;
            SPI1_RECEIVE_ENABLE = INT_ENABLED;
            SPI1_TRANSFER_ENABLE = INT_ENABLED;
            break;
        case E_SPI1_EEPROM_WAIT_READ_STAT_OK:
            if (SPI1STATbits.SPIBUSY)
                break;
            eeprom_read_byte = SPI1BUF;
            SPI1CONbits.MODE16 = 0;
            SPI1_state = E_SPI1_EEPROM_READ_DATA;
            SPI1BUF = 0x00;
            SPI1_RECEIVE_ENABLE = INT_ENABLED;
            SPI1_TRANSFER_ENABLE = INT_ENABLED;
            break;
        case E_SPI1_EEPROM_READ_DATA :
            if (SPI1STATbits.SPIBUSY)
                break;
            eeprom_buf[eeprom_buf_i++] = SPI1BUF;
            if (eeprom_buf_i == eeprom_buf_size)
                SPI1_state = E_SPI1_EEPROM_READ_DONE;
            else
                SPI1BUF = 0x00;
            SPI1_RECEIVE_ENABLE = INT_ENABLED;
            SPI1_TRANSFER_ENABLE = INT_ENABLED;
            break;
        case E_SPI1_EEPROM_READ_DONE:
            CS_EEPROM = 1;
            SPI1_state = E_SPI1_DONE;
            eeprom_buf_i = 0;
            SPI_eeprom_read_request = 0;
            break;
    }
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

void SPI1_eeprom_state_machine(void)
{
    if (SPI_eeprom_read_request)
	eeprom_state_machine_read();
    else if (SPI_eeprom_write_request)
	eeprom_state_machine_write();
}