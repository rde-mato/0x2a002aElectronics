#include <xc.h>
#include <sys/attribs.h>
#include "0x2a002a.h"

extern  u8  I2C2_state;
extern  u8  I2C2_read_request;
extern  u8  display_buf_dirty;
extern  u8  HT16_read_request;

void    process_I2C2_read_results(void)
{
   switch (I2C2_read_request)
        {
            case E_KEYSCAN:
                format_key_scan();
                process_key_scan();
                break;
        }
    I2C2_read_request = E_NONE;
    I2C2_state = E_I2C2_DONE;
}

void    process_I2C2_triggers()
{
    if (display_buf_dirty)
        led_refresh();
    else if (HT16_read_request)
         key_scan();
}

void    manage_I2C2(void)
{
    if (I2C2_state == E_I2C2_READ_RESULT)
        process_I2C2_read_results();
    if (I2C2_state == E_I2C2_DONE)
        process_I2C2_triggers();
}