#include <xc.h>
#include <sys/attribs.h>
#include "0x2a002a.h"


extern u8 SPI1_state;
extern u8 default_template;
extern u16 eeprom_address;
u8  SPI1_SD_RW;
u32 SD_return;
u8  SD_error = 0;
u8  SD_type = SD_SDSC;
u32 block = 0;
u32 retries = SD_RETRIES;
u8 SPI_SDCARD_read_request = 0;
u8 SPI_SDCARD_write_request = 0;
u8  SD_read_buf[SD_BLOCK_SIZE] = { 0 };
u16 SD_read_buf_index = 0;
u8  SD_write_buf[SD_BLOCK_SIZE] = "lorem ipsum";
u16 SD_write_buf_index = 0;
u32 SD_to_eeprom_block = 0;
size_t sd_to_eeprom_address = 0;
size_t eeprom_size = INSTRUMENTS_COUNT * PATTERNS_PER_INSTRUMENT * QTIME_PER_PATTERN * NOTES_PER_QTIME * ATTRIBUTES_PER_NOTE;

generic_callback    SD_read_callback = NULL;
generic_callback    SD_write_callback = NULL;
void    cb_copy_one_sd_block_to_eeprom(void);


void    send_lots_clk(void)
{
    u32 read32;
    u8  i = 20;

    SPI1CONbits.MODE32 = 1;
    while (--i)
    {
        SPI1BUF = 0xFFFFFFFF;
        while (SPI1STATbits.SPIBUSY) ;
        read32 = SPI1BUF;
    }
    SPI1CONbits.MODE32 = 0;
}

u8  SD_command_retry(u8 cmd, u32 arg, u8 crc, u8 restype)
{
    u8  read8;
    u32 read32;
    u8  ret = 0xFF;
    u32  retries = SD_RETRIES;

    CS_SD = 0x0;
    SPI1BUF = 0x40 | cmd;
    while (SPI1STATbits.SPIBUSY) ;
    read8 = SPI1BUF;

    SPI1CONbits.MODE32 = 1;

    SPI1BUF = arg;
    while (SPI1STATbits.SPIBUSY) ;
    read32 = SPI1BUF;

    SPI1CONbits.MODE32 = 0;

    SPI1BUF = crc;
    while (SPI1STATbits.SPIBUSY) ;
    read8 = SPI1BUF;

    while (retries--)
    {
        SPI1BUF = 0xFF;
        while (SPI1STATbits.SPIBUSY) ;
        if ((ret = SPI1BUF) != 0xFF)
            break;
    }
    if (ret != 0xFF && (restype == SD_R3 || restype == SD_R7))
    {
        SPI1CONbits.MODE32 = 1;
        SPI1BUF = 0xFFFFFFFF;
        while (SPI1STATbits.SPIBUSY) ;
        SD_return = SPI1BUF;
        SPI1CONbits.MODE32 = 0;
    }

    CS_SD = 0x1;
    SPI1BUF = 0xFF;
    while (SPI1STATbits.SPIBUSY) ;
    read8 = SPI1BUF;
    return (ret);
}

void SD_card_init(void)
{
    u8  R1_response;
    u32  retries = SD_RETRIES;

    // baisser le baud rate du spi
//    SPI1BRG = 9; //divise la frequence de SCK par 20 = 2 * (9 + 1)
    SPI1BRG = 3; //divise la frequence de SCK par 6 = 2 * (2 + 1)
    send_lots_clk();
    R1_response = SD_command_retry(0, 0x0,   0x95, SD_R1);
    R1_response = SD_command_retry(8, 0x1AA, 0x87, SD_R7);
    R1_response = SD_command_retry(58, 0x0,  0x01, SD_R3);  // reads the OCR register to know the voltage and therefore if the card is SDSC, SDHC or SDXC

    while (retries--)
    {
        R1_response = SD_command_retry(55, 0x0,  0x65, SD_R1);
        R1_response = SD_command_retry(41, 0x0,  0xE5, SD_R1);
        if (R1_response == 0x00)
            break ;
    }
    R1_response = SD_command_retry(58, 0x0, 0x01, SD_R3);  // reads the OCR register, if bit 30 of OCR is 0 then SDSC else SDHC or SDXC
    if (SD_return & (1 << 30))
        SD_type = SD_SDHC;
    R1_response = SD_command_retry(16, SD_BLOCK_SIZE, 0x01, SD_R1);  // set block length to 512
    // remonter le baud rate du spi
    SPI1BRG = 0; //set baudrate 1Mhz suivant 8 Mhz du pbclk
}

void    SD_card_read_state_machine(void)
{
    u8  read8;
    u32 read32;

    switch (SPI1_state)
    {
        case E_SPI1_SDCARD_READ_INIT:
            read8 = SPI1BUF;
            CS_SD = CS_LINE_DOWN;
            SPI1BUF = SD_CMD_READ_SINGLE_BLOCK;
            SPI1_state = E_SPI1_SDCARD_READ_SEND_BLOCK_ADDRESS;
            SPI1_TRANSMIT_ENABLE = INT_ENABLED;
            break;

        case E_SPI1_SDCARD_READ_SEND_BLOCK_ADDRESS:
            read8 = SPI1BUF;
            SPI1CONbits.MODE32 = 1;
            if (SD_type == SD_SDSC)
                block *= SD_BLOCK_SIZE;
            SPI1BUF = block;
            SPI1_state = E_SPI1_SDCARD_READ_SEND_DUMMY_CRC_AND_START_RETRIES;
            SPI1_TRANSMIT_ENABLE = INT_ENABLED;
            break;

        case E_SPI1_SDCARD_READ_SEND_DUMMY_CRC_AND_START_RETRIES:
            read32 = SPI1BUF;
            SPI1CONbits.MODE32 = 0;
            SPI1BUF = SD_CMD_DUMMY_CRC;
            retries = SD_RETRIES;
            SPI1_state = E_SPI1_SDCARD_READ_RETRY_TILL_NOT_BUSY;
            SPI1_TRANSMIT_ENABLE = INT_ENABLED;
            break;

        case E_SPI1_SDCARD_READ_RETRY_TILL_NOT_BUSY:
            read8 = SPI1BUF;
            if (read8 == 0x00)
            {
                retries = SD_RETRIES;
                SPI1_state = E_SPI1_SDCARD_READ_RETRY_TILL_TOKEN;
                SPI1BUF = 0xFF;
            }
            else if (retries)
            {
                SPI1BUF = 0xFF;
                retries--;
            }
            else
            {
                SPI1_state = E_SPI1_SDCARD_READ_ERROR;
                SD_error = SD_READ_ERROR_TOO_MANY_RETRIES;
            }
            SPI1_TRANSMIT_ENABLE = INT_ENABLED;
            break;

        case E_SPI1_SDCARD_READ_RETRY_TILL_TOKEN:
            read8 = SPI1BUF;
            if (read8 != 0xFF)
            {
                if (read8 == 0b11111110)
                {
                    SPI1_state = E_SPI1_SDCARD_READ_GET_ONE_BYTE;
                    SPI1BUF = 0xFF;
                }
                else
                {
                    SPI1_state = E_SPI1_SDCARD_READ_ERROR;
                    SD_error = SD_READ_ERROR_WRONG_TOKEN ;
                }
            }
            else if (retries)
            {
                SPI1BUF = 0xFF;
                retries--;
            }
            else
            {
                SPI1_state = E_SPI1_SDCARD_READ_ERROR;
                SD_error = SD_READ_ERROR_TOO_MANY_RETRIES;
            }
            SPI1_TRANSMIT_ENABLE = INT_ENABLED;
            break;

        case E_SPI1_SDCARD_READ_GET_ONE_BYTE:
            read8 = SPI1BUF;
            if (SD_read_buf_index >= SD_BLOCK_SIZE)
                SPI1_state = E_SPI1_SDCARD_READ_SEND_FINAL_FFS_1;
            else
                SD_read_buf[SD_read_buf_index++] = read8;
            SPI1BUF = 0xFF;
            SPI1_TRANSMIT_ENABLE = INT_ENABLED;
            break;

        case E_SPI1_SDCARD_READ_SEND_FINAL_FFS_1:
            read8 = SPI1BUF;
            SPI1BUF = 0xFF;
            SPI1_state = E_SPI1_SDCARD_READ_SEND_FINAL_FFS_2;
            SPI1_TRANSMIT_ENABLE = INT_ENABLED;
            break;

        case E_SPI1_SDCARD_READ_SEND_FINAL_FFS_2:
            read8 = SPI1BUF;
            SPI1BUF = 0xFF;
            SPI1_state = E_SPI1_SDCARD_READ_RELASE;
            SPI1_TRANSMIT_ENABLE = INT_ENABLED;
            break;

        case E_SPI1_SDCARD_READ_RELASE:
            read8 = SPI1BUF;
            CS_SD = CS_LINE_UP;
            SPI1BUF = 0xFF;
            SPI1_state = E_SPI1_SDCARD_READ_LAST_FF;
            SPI1_TRANSMIT_ENABLE = INT_ENABLED;
            break;

        case E_SPI1_SDCARD_READ_ERROR:
            read8 = SPI1BUF;
            CS_SD = CS_LINE_UP;
            SPI1_state = E_SPI1_DONE;
            request_template(TEMPLATE_SD_ERROR);
            SPI_SDCARD_read_request = 0;
            break;

        case E_SPI1_SDCARD_READ_LAST_FF:
            read8 = SPI1BUF;
            SPI1_state = E_SPI1_DONE;
            SD_read_buf_index = 0;
            SPI_SDCARD_read_request = 0;
            if (SD_read_callback != NULL)
                (*SD_read_callback)();
            break;
    }

}


u8  SD_card_read_block(u32 block_address, generic_callback cb)
{
    SD_read_callback = cb;
    SPI_SDCARD_read_request = 1;
    block = block_address;
}



void    SD_card_write_state_machine(void)
{
    u8  read8;
    u8  response_status;
    u32 read32;

    switch (SPI1_state)
    {
        case E_SPI1_SDCARD_WRITE_INIT:
            read8 = SPI1BUF;
            CS_SD = CS_LINE_DOWN;
            SPI1BUF = SD_CMD_WRITE_SINGLE_BLOCK;
            SPI1_state = E_SPI1_SDCARD_WRITE_SEND_BLOCK_ADDRESS;
            SPI1_TRANSMIT_ENABLE = INT_ENABLED;
            break;

        case E_SPI1_SDCARD_WRITE_SEND_BLOCK_ADDRESS:
            read8 = SPI1BUF;
            SPI1CONbits.MODE32 = 1;
            if (SD_type == SD_SDSC)
                block *= SD_BLOCK_SIZE;
            SPI1BUF = block;
            SPI1_state = E_SPI1_SDCARD_WRITE_SEND_DUMMY_CRC_AND_START_RETRIES;
            SPI1_TRANSMIT_ENABLE = INT_ENABLED;
            break;

        case E_SPI1_SDCARD_WRITE_SEND_DUMMY_CRC_AND_START_RETRIES:
            read32 = SPI1BUF;
            SPI1CONbits.MODE32 = 0;
            SPI1BUF = SD_CMD_DUMMY_CRC;
            retries = SD_RETRIES;
            SPI1_state = E_SPI1_SDCARD_WRITE_RETRY_TILL_NOT_BUSY;
            SPI1_TRANSMIT_ENABLE = INT_ENABLED;
            break;


        case E_SPI1_SDCARD_WRITE_RETRY_TILL_NOT_BUSY:
            read8 = SPI1BUF;
            if (read8 != 0xFF)
            {
                if (read8 == 0x00)
                {
                    retries = SD_RETRIES;
                    SPI1_state = E_SPI1_SDCARD_WRITE_SEND_START_TOKEN;
                    SPI1BUF = 0xFF;
                }
                else
                {
                    SPI1_state = E_SPI1_SDCARD_WRITE_ERROR;
                    SD_error = SD_WRITE_ERROR_WRONG_R1;
                }
            }
            else if (retries)
            {
                SPI1BUF = 0xFF;
                retries--;
            }
            else
            {
                SPI1_state = E_SPI1_SDCARD_WRITE_ERROR;
                SD_error = SD_WRITE_ERROR_TOO_MANY_RETRIES;
            }
            SPI1_TRANSMIT_ENABLE = INT_ENABLED;
            break;


        case E_SPI1_SDCARD_WRITE_WAIT_ONE_BYTE:
            read8 = SPI1BUF;
            SPI1BUF = 0xFF;
            SPI1_state = E_SPI1_SDCARD_WRITE_SEND_START_TOKEN;
            SPI1_TRANSMIT_ENABLE = INT_ENABLED;
            break;


        case E_SPI1_SDCARD_WRITE_SEND_START_TOKEN:
            read8 = SPI1BUF;
            SPI1BUF = 0b11111110;
            SPI1_state = E_SPI1_SDCARD_WRITE_SEND_ONE_BYTE;
            SPI1_TRANSMIT_ENABLE = INT_ENABLED;
            break;


        case E_SPI1_SDCARD_WRITE_SEND_ONE_BYTE:
            read8 = SPI1BUF;
            SPI1BUF = SD_write_buf[SD_write_buf_index++];
            if (SD_write_buf_index > SD_BLOCK_SIZE)
            {
                retries = SD_RETRIES;
                SPI1_state = E_SPI1_SDCARD_WRITE_RETRY_TILL_DATA_RESPONSE;
            }
            SPI1_TRANSMIT_ENABLE = INT_ENABLED;
            break;


        case E_SPI1_SDCARD_WRITE_RETRY_TILL_DATA_RESPONSE:
            read8 = SPI1BUF;
            if (read8 != 0xFF)
            {
                if ((read8 & 0x10) & !(read8 & 1))
                {
                    SPI1_state = E_SPI1_SDCARD_WRITE_ERROR;
                    SD_error = SD_WRITE_ERROR_WRONG_DATA_RESPONSE;
                }
                response_status = (read8 >> 1) & 0b111;
                if (response_status == 0b010)
                {
                    SPI1BUF = 0xFF;
                    SPI1_state = E_SPI1_SDCARD_WRITE_SEND_FF_TILL_END_OF_00;
                }
                else
                {
                    SPI1_state = E_SPI1_SDCARD_WRITE_ERROR;
                    SD_error = SD_WRITE_ERROR_WRONG_DATA_RESPONSE; // a  ameliorer
                }
            }
            else if (retries)
            {
                SPI1BUF = 0xFF;
                retries--;
            }
            else
            {
                SPI1_state = E_SPI1_SDCARD_WRITE_ERROR;
                SD_error = SD_WRITE_ERROR_TOO_MANY_RETRIES;
            }
            SPI1_TRANSMIT_ENABLE = INT_ENABLED;
            break;

        case E_SPI1_SDCARD_WRITE_SEND_FF_TILL_END_OF_00:
            read8 = SPI1BUF;
            if (read8 == 0x00)
            {
                SPI1_state = E_SPI1_SDCARD_WRITE_RELASE;
                SPI1BUF = 0xFF;
            }
            else if (retries)
            {
                SPI1BUF = 0xFF;
                retries--;
            }
            else
            {
                SPI1_state = E_SPI1_SDCARD_WRITE_ERROR;
                SD_error = SD_WRITE_ERROR_TOO_MANY_RETRIES;
            }
            SPI1_TRANSMIT_ENABLE = INT_ENABLED;
            break;


        case E_SPI1_SDCARD_WRITE_RELASE:
            read8 = SPI1BUF;
            CS_SD = CS_LINE_UP;
            SPI1BUF = 0xFF;
            SPI1_state = E_SPI1_SDCARD_WRITE_LAST_FF;
            SPI1_TRANSMIT_ENABLE = INT_ENABLED;
            break;

        case E_SPI1_SDCARD_WRITE_ERROR:
            read8 = SPI1BUF;
            CS_SD = CS_LINE_UP;
            SPI1_state = E_SPI1_DONE;
            request_template(TEMPLATE_SD_ERROR);
            SPI_SDCARD_write_request = 0;
            break;

        case E_SPI1_SDCARD_WRITE_LAST_FF:
            read8 = SPI1BUF;
            SPI1_state = E_SPI1_DONE;
            SD_write_buf_index = 0;
            SPI_SDCARD_write_request = 0;
            break;

    }
}


u8  SD_card_write_block(u32 block_address, generic_callback cb)
{
    SD_write_callback = cb;
    SPI_SDCARD_write_request = 1;
    block = block_address;
}



void    copy_EEPROM_to_SD(void)
{
    u32  address;
    u32  count;
    u32  bl;
    u8  read;
    u8  sd_errcode;

    address = 0;
    bl = 0;
    while (address < eeprom_size)
    {
        CS_EEPROM = CS_LINE_DOWN;
        SPI1BUF = E_EEPROM_READ;
        while (SPI1STATbits.SPIBUSY);
        read = SPI1BUF;

        SPI1BUF = (u8)(address >> 8);
        while (SPI1STATbits.SPIBUSY);
        read = SPI1BUF;
        SPI1BUF = (u8)(address);
        while (SPI1STATbits.SPIBUSY);
        read = SPI1BUF;

        count = 0;
        while (count < SD_BLOCK_SIZE)
        {
            SPI1BUF = 0xFF;
            while (SPI1STATbits.SPIBUSY);
            SD_write_buf[count++] = SPI1BUF;
        }
        CS_EEPROM = CS_LINE_UP;
        address += SD_BLOCK_SIZE;
        if ((sd_errcode = SD_card_write_block(bl++, NULL)) != SD_WRITE_NO_ERROR)
            return ;
    }
}

void    copy_SD_to_EEPROM(void)
{
    if (SPI_SDCARD_write_request == 0)
    {
        sd_to_eeprom_address = 0;
        SD_card_read_block(SD_to_eeprom_block++, &cb_copy_one_sd_block_to_eeprom);
    }

//                    default_template = TEMPLATE_DEFAULT;
//                    request_template(TEMPLATE_LOADING_SUCCESSFUL);
}

void    gpio_init_SD_card(void)
{
    SD_PRESENCE_GPIO = GPIO_INPUT;
}