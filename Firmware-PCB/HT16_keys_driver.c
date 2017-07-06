#include <xc.h>
#include <sys/attribs.h>
#include "0x2a002a.h"


u8      HT16_read_keys_request = 0;
u32     current_key_scan = 0;
u32     previous_key_scan = 0;

u32     poll_count = 0;
u32     buttons_timers[32] = {0};

extern u8	I2C1_read_buf[];

void    int_init_HT16_press(void)
{
    HT16_INT_POLARITY = FALLING_EDGE;
    HT16_INT_FLAG_CLR;
    HT16_INT_PRIORITY = 2;
    HT16_INT_ENABLE = INT_ENABLED;
}

void __ISR(_EXTERNAL_0_VECTOR, IPL2AUTO) HT16IntHandler(void) {
    HT16_read_keys_request = 1;
    TMR5 = 0;
    ++poll_count;
    T5CONbits.ON = 1;
    HT16_INT_FLAG_CLR;
}

void    int_init_timer5(void)
{
    TIMER5_INT_FLAG_CLR;
    TIMER5_INT_PRIORITY = 3;
    TIMER5_INT_ENABLE = INT_ENABLED;
}

void    timer_5_init(void)
{
    TIMER5_STOP_AND_RESET;
    TIMER5_VALUE = 0;
    TIMER5_PRESCALE = TIMER_PRESCALE_4;
    TIMER5_PERIOD = 64 * FREQUENCY / (8000 / BUTTON_POLL_DELAY_MS) - 1;
}

void __ISR(_TIMER_5_VECTOR, IPL3AUTO) Timer5Handler(void)
{
    IFS0bits.T5IF = 0;
    T5CONbits.ON = 0;
    current_key_scan = 0;
    HT16_read_keys_request = 1;
}

void process_key_scan(void)
{
    u32 		changed_buttons;
    u32 		unchanged_pressed_buttons;
    u32 		current_poll_count;
    u32 		newly_pressed_buttons;
    u32 		newly_released_buttons;
    u8  		main_encoder;
	static u8	previous_main_encoder = 0;
    u8  		i;

    current_key_scan = (I2C1_read_buf[0] << 0 )
                        | ((I2C1_read_buf[1] & 0b1111) << 8)
                        | (I2C1_read_buf[2] << 12)
                        | ((I2C1_read_buf[3] & 0b1111) << 20)
                        | (I2C1_read_buf[4] << 24);
    main_encoder = I2C1_read_buf[5] & 0b1;
    current_poll_count = poll_count;
    poll_count = 0;
    changed_buttons = current_key_scan ^ previous_key_scan;
    unchanged_pressed_buttons = current_key_scan & previous_key_scan;
    newly_pressed_buttons = current_key_scan & ~previous_key_scan;
    newly_released_buttons = ~current_key_scan & previous_key_scan;

    if (newly_pressed_buttons)
    {
        i = 0;
        while (i < 32)
        {
            if (newly_pressed_buttons & (1 << i))
                event_handler(E_KEY_PRESSED, i);
            ++i;
        }
    }
    if (newly_released_buttons)
    {
        i = 0;
        while (i < 32)
        {
            if (newly_released_buttons & (1 << i))
                event_handler(E_KEY_RELEASED, i);
            ++i;
        }
    }
    if (unchanged_pressed_buttons)
    {
        i = 0;
        while (i < 32)
        {
            if (unchanged_pressed_buttons & (1 << i))
            {
                buttons_timers[i] += current_poll_count;
                if(buttons_timers[i] > LONG_PRESS_LIMIT)
                {
                    event_handler(E_KEY_LONG_PRESSED, i);
                    buttons_timers[i] = 0;
                }
            }
            else
                buttons_timers[i] = 0;
            ++i;
        }
    }
    else
    {
        while (i < 32)
            buttons_timers[i++] = 0;
    }
    previous_key_scan = current_key_scan;

	if (!previous_main_encoder && main_encoder)
		event_handler(E_KEY_PRESSED, E_SOURCE_ENCODER_MAIN);
	previous_main_encoder = main_encoder;
}

void key_scan(void)
{
    HT16_read_keys_request = 0;
    I2C1_read_callback(0xE0, 0x40, 6, &process_key_scan);
}

