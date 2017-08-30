#include <xc.h>
#include <sys/attribs.h>
#include "0x2a002a.h"

extern struct s_all	all;
static u8			transfer_buffer[SD_BLOCK_SIZE];
static u16			cur_SD_block;
static u16			cur_eeprom_address;
static const size_t	eeprom_size = INSTRUMENTS_COUNT * PATTERNS_PER_INSTRUMENT * QTIME_PER_PATTERN * NOTES_PER_QTIME * ATTRIBUTES_PER_NOTE;

void				cb_copy_eeprom_to_sd_after_sd_write(void);
void				cb_copy_eeprom_to_sd_after_eeprom_read(void);
void				cb_copy_sd_to_eeprom_after_eeprom_write(void);
void				cb_copy_sd_to_eeprom_after_sd_read(void);

void	cb_copy_eeprom_to_sd_after_sd_write(void)
{
    cur_SD_block++;
    if (cur_SD_block * SD_BLOCK_SIZE >= eeprom_size)
    {
        request_template(TEMPLATE_LOADING_SUCCESSFUL);
	update_mode(E_MODE_PATTERN);
        update_leds_base_case();
    }
    else
    {
        all.loading_percentage = 100 * cur_eeprom_address / eeprom_size;
	eeprom_read_callback(
		cur_eeprom_address,
		(u8 *)transfer_buffer,
		SD_BLOCK_SIZE,
		&cb_copy_eeprom_to_sd_after_eeprom_read);
        request_template(TEMPLATE_LOADING_IN_PROGRESS);
    }
}


void	cb_copy_eeprom_to_sd_after_eeprom_read(void)
{
    cur_eeprom_address += SD_BLOCK_SIZE;
    SD_write_block_callback(
        (u8 *)transfer_buffer,
    	cur_SD_block,
        SD_BLOCK_SIZE,
	&cb_copy_eeprom_to_sd_after_sd_write);
}

void	copy_EEPROM_to_SD(void)
{
    if (SPI1Fbits.eeprom_r == FALSE)
    {
        cur_SD_block = 0 ;
        cur_eeprom_address = 0;
        all.loading_percentage = 0;
	eeprom_read_callback(
		cur_eeprom_address,
		(u8 *)transfer_buffer,
		SD_BLOCK_SIZE,
		&cb_copy_eeprom_to_sd_after_eeprom_read);
    }
}


////////////////////////////////////////////////////////////////////////////////

void	cb_copy_sd_to_eeprom_after_eeprom_write(void)
{
    if (cur_eeprom_address >= eeprom_size - SD_BLOCK_SIZE)
    {
        initial_eeprom_download();
        request_template(TEMPLATE_LOADING_SUCCESSFUL);
    }
    else
    {
        cur_eeprom_address += SD_BLOCK_SIZE;
        all.loading_percentage = 100 * cur_eeprom_address / eeprom_size;
        request_template(TEMPLATE_LOADING_IN_PROGRESS);
        SD_read_block_callback(
                cur_SD_block,
                (u8 *)transfer_buffer,
                SD_BLOCK_SIZE,
                &cb_copy_sd_to_eeprom_after_sd_read);
    }
}

void	cb_copy_sd_to_eeprom_after_sd_read(void)
{
    cur_SD_block++;
    eeprom_write_callback(
            (u8 *)transfer_buffer,
            cur_eeprom_address,
            SD_BLOCK_SIZE,
            &cb_copy_sd_to_eeprom_after_eeprom_write);
}

void	copy_SD_to_EEPROM(void)
{
    if (SPI1Fbits.SD_w == FALSE)
    {
        cur_SD_block = 0 ;
        cur_eeprom_address = 0;
        all.loading_percentage = 0;
        SD_read_block_callback(
                cur_SD_block,
                (u8 *)transfer_buffer,
                SD_BLOCK_SIZE,
                &cb_copy_sd_to_eeprom_after_sd_read);
    }
}
