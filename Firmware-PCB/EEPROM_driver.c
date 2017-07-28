#include <xc.h>
#include <sys/attribs.h>
#include "0x2a002a.h"

extern u8   SPI1_state;
extern u8   cur_instrument;
extern u8   cur_pattern;
extern u8   active_patterns_array[INSTRUMENTS_COUNT][QTIME_PER_PATTERN][NOTES_PER_QTIME][ATTRIBUTES_PER_NOTE];
extern u8   cur_active_pattern[QTIME_PER_PATTERN][NOTES_PER_QTIME][ATTRIBUTES_PER_NOTE];
extern u8   active_instrument[PATTERNS_PER_INSTRUMENT][QTIME_PER_PATTERN][NOTES_PER_QTIME][ATTRIBUTES_PER_NOTE];
extern u8   current_mode;
extern u8  SD_read_buf[SD_BLOCK_SIZE];
extern u32 SD_to_eeprom_block;
extern size_t sd_to_eeprom_address;
extern size_t eeprom_size;
extern generic_callback    SD_write_callback;
extern u8 SPI_SDCARD_write_request;
extern u8  SD_write_buf[SD_BLOCK_SIZE];
extern u16 SD_write_buf_index;

u8          SPI_eeprom_write_request = 0;
u8          SPI_eeprom_read_request = 0;
u16         eeprom_address_start;
u32         eeprom_read_size;
u8          *eeprom_read_dest;
u32         eeprom_read_index = 0;
u8          eeprom_write_buf[EEPROM_BLOCK_SIZE];
u32         eeprom_write_size;
u32         eeprom_write_index = 0;
u8          eeprom_buf_size;
u8          eeprom_buf[EEPROM_BLOCK_SIZE];
u8          loading_percentage = 0;
size_t      eeprom_to_sd_address = 0;
generic_callback    eeprom_write_callback = NULL;
generic_callback    eeprom_read_callback = NULL;

const   size_t pattern_size = QTIME_PER_PATTERN * NOTES_PER_QTIME * ATTRIBUTES_PER_NOTE;
const   size_t instrument_size = PATTERNS_PER_INSTRUMENT * QTIME_PER_PATTERN * NOTES_PER_QTIME * ATTRIBUTES_PER_NOTE;

void    cb_copy_sd_to_eeprom_after_eeprom_write(void);
void    cb_copy_sd_to_eeprom_after_sd_read(void);
void    cb_copy_eeprom_to_sd_after_eeprom_read(void);

void    initial_eeprom_download(void)
{
    u8  i = 0;
    u8  p = 0;
    u8  qt;
    u8  n;
    u8  read;
    u16 address;

    CS_EEPROM = CS_LINE_DOWN;
    SPI1BUF = E_EEPROM_READ;
    while (SPI1STATbits.SPIBUSY);
    read = SPI1BUF;

    address = 0x0000;
    SPI1BUF = (u8)(address >> 8);
    while (SPI1STATbits.SPIBUSY);
    read = SPI1BUF;
    SPI1BUF = (u8)(address);
    while (SPI1STATbits.SPIBUSY);
    read = SPI1BUF;

    while (p < PATTERNS_PER_INSTRUMENT)
    {
        qt = 0;
        while (qt < QTIME_PER_PATTERN)
        {
            n = 0;
            while (n < NOTES_PER_QTIME)
            {
                SPI1BUF = 0xFF;
                while (SPI1STATbits.SPIBUSY);
                active_instrument[p][qt][n][0] = SPI1BUF;
                SPI1BUF = 0xFF;
                while (SPI1STATbits.SPIBUSY);
                active_instrument[p][qt][n][1] = SPI1BUF;
                ++n;
            }
            ++qt;
        }
        ++p;
    }
    CS_EEPROM = CS_LINE_UP;

    i = 0;
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
    memcpy(cur_active_pattern, active_patterns_array[cur_instrument][cur_pattern], pattern_size);
    update_leds_base_case();
}

void    update_after_pattern_change(void)
{
    memcpy(cur_active_pattern, active_instrument[cur_pattern], pattern_size);
}


void    cb_save_cur_pattern_to_eeprom(void)
{
    eeprom_write_callback = NULL;

    if (eeprom_write_index >= eeprom_write_size)
    {
        eeprom_write_index = 0;
        SPI1_state = E_SPI1_DONE;
        eeprom_write_size = 0;
        eeprom_write_index = 0;
        SPI_eeprom_write_request = 0;
        eeprom_address_start = 0;
        request_template(TEMPLATE_PATTERN_RECORDED);
    }
    else
    {
        memcpy(eeprom_write_buf, (u8 *)cur_active_pattern + eeprom_write_index, EEPROM_BLOCK_SIZE);
        eeprom_address_start = (PATTERNS_PER_INSTRUMENT * cur_instrument + cur_pattern) * pattern_size + eeprom_write_index;
        SPI1_state = E_SPI1_EEPROM_CHECK_BUSY;
        eeprom_write_callback = &cb_save_cur_pattern_to_eeprom;
    }
}


void    save_cur_pattern_to_eeprom(void)
{
    memcpy(active_instrument[cur_pattern], cur_active_pattern, pattern_size);
    memcpy(active_patterns_array[cur_instrument], cur_active_pattern, pattern_size);
    eeprom_address_start = (PATTERNS_PER_INSTRUMENT * cur_instrument + cur_pattern) * pattern_size + eeprom_write_index;
    eeprom_write_size = pattern_size;
    eeprom_write_index = 0;
    eeprom_write_callback = &cb_save_cur_pattern_to_eeprom;
    memcpy(eeprom_write_buf, cur_active_pattern + eeprom_write_index, EEPROM_BLOCK_SIZE);
    SPI_eeprom_write_request = 1;
}

void    cb_cur_instrument_from_eeprom(void)
{
    eeprom_read_callback = NULL;
    SPI1_state = E_SPI1_DONE;
    eeprom_read_index = 0;
    eeprom_read_size = 0;
    SPI_eeprom_read_request = 0;
    memcpy(cur_active_pattern, active_instrument[cur_pattern], pattern_size);
    request_template(TEMPLATE_INSTRUMENT);
    current_mode = E_MODE_PATTERN;
    update_leds_base_case();
}

void    load_cur_instrument_from_eeprom(void)
{
    eeprom_address_start = cur_instrument * instrument_size;
    eeprom_read_dest = (u8 *)active_instrument;
    eeprom_read_size = instrument_size;
    eeprom_read_callback = &cb_cur_instrument_from_eeprom;
    SPI_eeprom_read_request = 1;
}

void    active_instrument_init(void)
{
    load_cur_instrument_from_eeprom();
}


void    enable_spi_interrupt(void)
{
    SPI1_TRANSMIT_ENABLE = INT_ENABLED;
}

void	eeprom_state_machine_write(void)
{
    u8          read8;

    switch (SPI1_state)
    {
        case E_SPI1_EEPROM_CHECK_BUSY:
            read8 = SPI1BUF;
            CS_EEPROM = CS_LINE_DOWN;
            SPI1BUF = E_EEPROM_RDSR;
            SPI1_state = E_SPI1_EEPROM_CHECK_BUSY_2;
            SPI1_TRANSMIT_ENABLE = INT_ENABLED;
            break ;

        case E_SPI1_EEPROM_CHECK_BUSY_2:
            read8 = SPI1BUF;
            SPI1BUF = 0xFF;
            SPI1_state = E_SPI1_EEPROM_CHECK_BUSY_3;
            SPI1_TRANSMIT_ENABLE = INT_ENABLED;
            break ;

        case E_SPI1_EEPROM_CHECK_BUSY_3:
            read8 = SPI1BUF;
            SPI1BUF = 0xFF;
            if (((read8 ^ 0b10) & 0b01) == 0)
                SPI1_state = E_SPI1_EEPROM_WRITE_ENABLE_1;
            else
                SPI1_state = E_SPI1_EEPROM_CHECK_BUSY;
            CS_EEPROM = CS_LINE_UP;
            SPI1_TRANSMIT_ENABLE = INT_ENABLED;
//            timer4_push(100, &enable_spi_interrupt);
            break ;

        case E_SPI1_EEPROM_WRITE_ENABLE_1:
            read8 = SPI1BUF;
            CS_EEPROM = CS_LINE_DOWN;
            SPI1BUF = E_EEPROM_WREN;
            SPI1_state = E_SPI1_EEPROM_WRITE_ENABLE_2;
            SPI1_TRANSMIT_ENABLE = INT_ENABLED;
            break ;

        case E_SPI1_EEPROM_WRITE_ENABLE_2:
            read8 = SPI1BUF;
            SPI1BUF = 0xFF;
            CS_EEPROM = CS_LINE_UP;
            SPI1_state = E_SPI1_EEPROM_CHECK_WREN;
            SPI1_TRANSMIT_ENABLE = INT_ENABLED;
//            timer4_push(100, &enable_spi_interrupt);
            break ;

        case E_SPI1_EEPROM_CHECK_WREN:
            read8 = SPI1BUF;
            CS_EEPROM = CS_LINE_DOWN;
            SPI1BUF = E_EEPROM_RDSR;
            SPI1_state = E_SPI1_EEPROM_CHECK_WREN_2;
            SPI1_TRANSMIT_ENABLE = INT_ENABLED;
            break ;

        case E_SPI1_EEPROM_CHECK_WREN_2:
            read8 = SPI1BUF;
            SPI1BUF = 0xFF;
            SPI1_state = E_SPI1_EEPROM_CHECK_WREN_3;
            SPI1_TRANSMIT_ENABLE = INT_ENABLED;
            break ;

        case E_SPI1_EEPROM_CHECK_WREN_3:
            read8 = SPI1BUF;
            SPI1BUF = 0xFF;
            if (((read8 ^ 0b10) & 0b11) == 0)
                SPI1_state = E_SPI1_EEPROM_WRITE_INSTRUCTION;
            else
                SPI1_state = E_SPI1_EEPROM_WRITE_ENABLE_1;
            CS_EEPROM = CS_LINE_UP;
            SPI1_TRANSMIT_ENABLE = INT_ENABLED;
//            timer4_push(100, &enable_spi_interrupt);
            break ;

        case E_SPI1_EEPROM_WRITE_INSTRUCTION:
            CS_EEPROM = CS_LINE_DOWN;
            read8 = SPI1BUF;
            SPI1_state = E_SPI1_EEPROM_WRITE_ADDRESS_FIRST_BYTE;
            SPI1BUF = E_EEPROM_WRITE;
            SPI1_TRANSMIT_ENABLE = INT_ENABLED;
            break ;

        case E_SPI1_EEPROM_WRITE_ADDRESS_FIRST_BYTE:
            read8 = SPI1BUF;
            SPI1_state = E_SPI1_EEPROM_WRITE_ADDRESS_SECOND_BYTE;
            SPI1BUF = (u8)(eeprom_address_start >> 8);
            SPI1_TRANSMIT_ENABLE = INT_ENABLED;
            break ;

        case E_SPI1_EEPROM_WRITE_ADDRESS_SECOND_BYTE:
            read8 = SPI1BUF;
            SPI1_state = E_SPI1_EEPROM_WRITE_DATA;
            SPI1BUF = (u8)(eeprom_address_start);
            SPI1_TRANSMIT_ENABLE = INT_ENABLED;
            break ;

        case E_SPI1_EEPROM_WRITE_DATA:
            read8 = SPI1BUF;
            if (eeprom_write_index >= eeprom_write_size - 1 || (eeprom_address_start & 127) == 127)
            {
                SPI1_state = E_SPI1_EEPROM_WRITE_PAGE_DONE;
                SPI1BUF = eeprom_write_buf[eeprom_write_index++ & 127];
                SPI1_TRANSMIT_ENABLE = INT_ENABLED;
            }
            else
            {
                SPI1BUF = eeprom_write_buf[eeprom_write_index++ & 127];
                SPI1_TRANSMIT_ENABLE = INT_ENABLED;
            }
            eeprom_address_start++;
            break ;

        case E_SPI1_EEPROM_WRITE_PAGE_DONE:
            read8 = SPI1BUF;
            CS_EEPROM = CS_LINE_UP;
            SPI1_state = E_SPI1_EEPROM_WRITE_CALLBACK;
            timer4_push(25, &enable_spi_interrupt);

        case E_SPI1_EEPROM_WRITE_CALLBACK:

            if (eeprom_write_callback != NULL)
                (*eeprom_write_callback)();
            else
            {
                SPI1_state = E_SPI1_DONE;
                eeprom_write_size = 0;
                eeprom_write_index = 0;
                SPI_eeprom_write_request = 0;
            }
            break;
    }
}


void	eeprom_state_machine_read(void)
{
    u8  read8;

    switch (SPI1_state)
    {
        case E_SPI1_EEPROM_READ_INSTRUCTION:
            read8 = SPI1BUF;
            CS_EEPROM = CS_LINE_DOWN;
            SPI1_state = E_SPI1_EEPROM_READ_ADDRESS_FIRST_BYTE;
            SPI1BUF = E_EEPROM_READ;
            SPI1_TRANSMIT_ENABLE = INT_ENABLED;
            break ;

        case E_SPI1_EEPROM_READ_ADDRESS_FIRST_BYTE:
            read8 = SPI1BUF;
            SPI1_state = E_SPI1_EEPROM_READ_ADDRESS_SECOND_BYTE;
            SPI1BUF = (u8)(eeprom_address_start >> 8);
            SPI1_TRANSMIT_ENABLE = INT_ENABLED;
            break ;

        case E_SPI1_EEPROM_READ_ADDRESS_SECOND_BYTE:
            read8 = SPI1BUF;
            SPI1_state = E_SPI1_EEPROM_READ_ADDRESS_START_READ;
            SPI1BUF = (u8)(eeprom_address_start);
            SPI1_TRANSMIT_ENABLE = INT_ENABLED;
            break ;

        case E_SPI1_EEPROM_READ_ADDRESS_START_READ:
            read8 = SPI1BUF;
            SPI1_state = E_SPI1_EEPROM_READ_DATA;
            SPI1BUF = 0xFF;
            SPI1_TRANSMIT_ENABLE = INT_ENABLED;
            break ;

        case E_SPI1_EEPROM_READ_DATA:
            eeprom_read_dest[eeprom_read_index++] = SPI1BUF;
            if (eeprom_read_index >= eeprom_read_size)
                SPI1_state = E_SPI1_EEPROM_READ_CALLBACK;
            else
            {
                SPI1BUF = 0xFF;
                SPI1_RECEIVE_ENABLE = INT_ENABLED;
                break ;
            }



        case E_SPI1_EEPROM_READ_CALLBACK:
            CS_EEPROM = CS_LINE_UP;

            if (eeprom_read_callback != NULL)
                (*eeprom_read_callback)();
            else
            {
                SPI1_state = E_SPI1_DONE;
                eeprom_read_size = 0;
                eeprom_read_index = 0;
                SPI_eeprom_read_request = 0;
            }
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

    no_notes_everywhere();
}



void    cb_copy_sd_to_eeprom_after_sd_read(void)
{
    eeprom_write_size = SD_BLOCK_SIZE;
    eeprom_write_index = 0;
    eeprom_write_callback = &cb_copy_sd_to_eeprom_after_eeprom_write;
    memcpy(eeprom_write_buf, SD_read_buf, EEPROM_BLOCK_SIZE);
    SPI_eeprom_write_request = 1;
}

void    cb_copy_sd_to_eeprom_after_eeprom_write(void)
{
    eeprom_write_callback = NULL;

    sd_to_eeprom_address += EEPROM_BLOCK_SIZE;
    if (eeprom_write_index >= eeprom_write_size)
    {
        eeprom_write_size = 0;
        eeprom_write_index = 0;
        SPI_eeprom_write_request = 0;
        SPI1_state = E_SPI1_DONE;
        if (sd_to_eeprom_address >= eeprom_size)
        {
            sd_to_eeprom_address = 0;
            SD_to_eeprom_block = 0;
            initial_eeprom_download();
            request_template(TEMPLATE_LOADING_SUCCESSFUL);
            loading_percentage = 0;
        }
        else
        {
            loading_percentage = 100 * sd_to_eeprom_address / eeprom_size;
            request_template(TEMPLATE_LOADING_IN_PROGRESS);
            SD_card_read_block(SD_to_eeprom_block++, &cb_copy_sd_to_eeprom_after_sd_read);
        }
    }
    else
    {
        memcpy(eeprom_write_buf, SD_read_buf + eeprom_write_index, EEPROM_BLOCK_SIZE);
        SPI1_state = E_SPI1_EEPROM_CHECK_BUSY;
        eeprom_write_callback = &cb_copy_sd_to_eeprom_after_eeprom_write;
    }
}


void    cb_copy_eeprom_to_sd_after_sd_write(void)
{
    eeprom_to_sd_address += SD_BLOCK_SIZE;
    SD_write_callback = NULL;
    SPI_SDCARD_write_request = 0;
    SD_write_buf_index = 0;
    SPI1_state = E_SPI1_DONE;

    if (eeprom_to_sd_address >= eeprom_size)
    {
        eeprom_to_sd_address = 0;
        eeprom_address_start = 0;
        eeprom_read_dest = NULL;
        eeprom_read_size = 0;
        request_template(TEMPLATE_LOADING_SUCCESSFUL);
        current_mode = E_MODE_PATTERN;
        loading_percentage = 0;
        update_leds_base_case();
    }
    else
    {
        loading_percentage = 100 * eeprom_to_sd_address / eeprom_size;
        eeprom_address_start = eeprom_to_sd_address;
        eeprom_read_dest = (u8 *)SD_write_buf;
        eeprom_read_size = SD_BLOCK_SIZE;
        eeprom_read_callback = &cb_copy_eeprom_to_sd_after_eeprom_read;
        SPI_eeprom_read_request = 1;
        request_template(TEMPLATE_LOADING_IN_PROGRESS);
    }

}


void    cb_copy_eeprom_to_sd_after_eeprom_read(void)
{
    eeprom_read_callback = NULL;
    SPI1_state = E_SPI1_DONE;
    eeprom_read_index = 0;
    eeprom_read_size = 0;
    SPI_eeprom_read_request = 0;
    SD_card_write_block(eeprom_to_sd_address / SD_BLOCK_SIZE, &cb_copy_eeprom_to_sd_after_sd_write);


}


void    copy_EEPROM_to_SD(void)
{
    if (SPI_eeprom_read_request == 0)
    {
        eeprom_to_sd_address = 0;
        eeprom_address_start = 0;
        eeprom_read_dest = (u8 *)SD_write_buf;
        eeprom_read_size = SD_BLOCK_SIZE;
        eeprom_read_callback = &cb_copy_eeprom_to_sd_after_eeprom_read;
        SPI_eeprom_read_request = 1;
    }
}