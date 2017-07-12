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
void    update_after_instrument_change(void)
{
    memcpy(cur_active_pattern, active_patterns_array[cur_instrument][cur_pattern], pattern_size);
}

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
                active_patterns_array[i][qt][n][0] = SPI1BUF;
                SPI1BUF = 0xFF;
                while (SPI1STATbits.SPIBUSY);
                active_patterns_array[i][qt][n][1] = SPI1BUF;
                ++n;
            }
            ++qt;
        }
        ++i;
        CS_EEPROM = CS_LINE_UP;
    }
    update_after_instrument_change();
}

void    update_after_pattern_change(void)
{
    memcpy(cur_active_pattern, active_instrument[cur_pattern], pattern_size);
}

void    save_cur_pattern_to_eeprom(void)
{
//    EEPROM_requested_instrument = instrument;
//    EEPROM_requested_pattern = pattern;
    memcpy(active_instrument[cur_pattern], cur_active_pattern, pattern_size);
    memcpy(active_patterns_array[cur_instrument], cur_active_pattern, pattern_size);
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
        case E_SPI1_EEPROM_WRITE_ENABLE:
            read8 = SPI1BUF;
            CS_EEPROM = CS_LINE_DOWN;
            SPI1BUF = E_EEPROM_WREN;
            SPI1_state = E_SPI1_EEPROM_WRITE_INSTRUCTION;
            SPI1_TRANSMIT_ENABLE = INT_ENABLED;
            break ;

        case E_SPI1_EEPROM_WRITE_INSTRUCTION:
            read8 = SPI1BUF;
            eeprom_cur_address = eeprom_address;
            CS_EEPROM = CS_LINE_UP;
            asm("nop");
            asm("nop");
            asm("nop");
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
            if ((eeprom_cur_address++ & 127) == 127 && eeprom_write_index < eeprom_write_size - 1)
            {
                SPI1BUF = eeprom_write_buf[eeprom_write_index++];
                eeprom_address = eeprom_cur_address;
                SPI1_state = E_SPI1_EEPROM_WRITE_ENABLE;
                SPI1_TRANSMIT_ENABLE = INT_ENABLED;
                CS_EEPROM = CS_LINE_UP;
                asm("nop");
                asm("nop");
                asm("nop");
                break ;
            }
            else if (eeprom_write_index >= eeprom_write_size - 1)
            {
                SPI1_state = E_SPI1_EEPROM_WRITE_DONE;
                SPI1BUF = eeprom_write_buf[eeprom_write_index++];
                SPI1_TRANSMIT_ENABLE = INT_ENABLED;
                break ;
            }
            else
            {
                SPI1BUF = eeprom_write_buf[eeprom_write_index++];
                SPI1_TRANSMIT_ENABLE = INT_ENABLED;
                break ;
            }


        case E_SPI1_EEPROM_WRITE_DONE:
            read8 = SPI1BUF;
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
            update_after_instrument_change();
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
void    eeprom_state_machine_write(void)
{
    u16         eeprom_read_byte;
    static u8   eeprom_buf_i = 0;

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
*/