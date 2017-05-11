#include <xc.h>
#include "tp.h"
#include <sys/attribs.h>

#define LONG_PRESS_LIMIT (16000)

u8      button_pressed = 0;
u8      key_scan_dirty = 0;
u8      button_buf[16];
u32     current_key_scan = 0;
u32     previous_key_scan = 0;

u32     short_press_buttons = 0;
u32     long_press_buttons = 0;
u32     newly_pressed_buttons = 0;
u32     newly_released_buttons = 0;
u8      button_interrupt_state = 0; /// a changer
extern u8 I2C2_state;
extern u8 I2C2_request;



const u32 buttonmatrix[16] = {
    0x80000000,
    0x10000000,
    0x04000000,
    0x00000400,
    0x20000000,
    0x40000000,
    0x01000000,
    0x02000000,
    0x08000000,
    0x00010000,
    0x00000001,
    0x00000200,
    0x00080000,
    0x00040000,
    0x00020000,
    0x00000002
};
extern  u8 I2C2_read_buf[READ_BUF_SIZE];
extern  u32  leds_status;


//extern void led_set(u8 index);

//void __ISR(_EXTERNAL_2_VECTOR, IPL2AUTO) Int2Handler(void) {
//    key_scan_dirty = 1;
//    IFS0bits.INT2IF = 0; // Reset the flag
//}


void __ISR(_EXTERNAL_2_VECTOR, IPL2AUTO) Int2Handler(void) {
    button_pressed = 1;

    IEC0bits.INT2IE = 0; // Disable interrupt for HT16
    IFS0bits.INT2IF = 0; // Reset the flag for HT16 interrupt

    TMR4 = 0;
//    IEC0bits.T5IE = 1; // enable interrupts for timer 4-5
    T4CONbits.ON = 1;
//    IFS0bits.T5IF = 1; // declenchement manuel de l'interrupt pour premiere iteration
}

void __ISR(_CORE_SOFTWARE_0_VECTOR, IPL2AUTO) ButtonActionsHandler(void) {
    if (button_interrupt_state == 2)
    {
        u8 i = 0;
        while (i < 32)
        {
            if (newly_released_buttons & (1 << i))
                led_toggle(i);
            i++;
        }
    }
    IFS0bits.CS0IF = 0;
}


void process_key_scan(void)
{
    u32 changed_buttons;
    u32 unchanged_pressed_buttons;
    u32 current_timer;
    u32 buttons_timers[32] = {0};
    u8  i;

    current_timer = TMR4;
    TMR4 = 0;
    changed_buttons = current_key_scan ^ previous_key_scan;
    unchanged_pressed_buttons = current_key_scan & previous_key_scan;
    newly_pressed_buttons = current_key_scan & ~previous_key_scan;
    newly_released_buttons = ~current_key_scan & previous_key_scan;

    if (newly_pressed_buttons)
    {
        button_interrupt_state = 1;
        IFS0bits.CS0IF = 1;// declencher un interrupt de type onbuttondown
    }
    if (newly_released_buttons)
    {
        button_interrupt_state = 2;
        IFS0bits.CS0IF = 1;// declencher un interrupt de type onbuttondown
    }
    i = 0;
    if (unchanged_pressed_buttons)
    {
        while (i < 32)
        {
            if (unchanged_pressed_buttons & (1 << i))
            {
                buttons_timers[i] += current_timer;
                if(buttons_timers[i] > LONG_PRESS_LIMIT)
                    long_press_buttons |= (1 << i);
            }
            else
                buttons_timers[i] = 0;
            ++i;
        }
        if (long_press_buttons)
        {
            button_interrupt_state = 3;
            IFS0bits.CS0IF = 1;// declencher un interrupt de type onbuttondown
        }
    }
    else            // pas forcement necessaire, a tester
    {
        while (i < 32)
            buttons_timers[i++] = 0;
    }
    if (!current_key_scan)
        end_input_polling();
    previous_key_scan = current_key_scan;
//    key_scan_dirty = 0;
}

void end_input_polling(void)
{
    IFS0bits.INT2IF = 0; // Reset the flag for HT16 interrupt - a priori inutile
//    IEC0bits.T5IE = 0; // Disable Timer interrupt
    T4CONbits.ON = 0;
    TMR4 = 0;  // probablement inutile
    previous_key_scan = 0;
    current_key_scan = 0;
    button_pressed = 0;
    IEC0bits.INT2IE = 1; // Re-enable interrupt for HT16
}

void read_key_scan(void)
{
    I2C2_request = E_KEYSCAN;
    I2C2_push(0xE0);
    I2C2_push(0x40);
    I2C2_read(6);
}

void format_key_scan(void)
{
    u32 ks;
    u8 i;

    ks = (I2C2_read_buf[0] << 24 )| (I2C2_read_buf[1] << 16) | (I2C2_read_buf[2] << 8) | I2C2_read_buf[3] ;
    i = 0;
    current_key_scan = 0;
    while (i < 16)
    {
        if (buttonmatrix[i] & ks)
            current_key_scan |= (1 << (i));
        ++i;
    }
  //  key_scan_dirty = 1;
}


//void execute_changes_in_button_status(void)
//{
//     u8  i;
//     u32 old_button;
//     u32 new_button;
//
//    i = 0;
//    while (i < 16)
//    {
//        new_button = new_button_status & (1 << i);
//        old_button = old_button_status & (1 << i);
//        if (new_button && (new_button != old_button))
//            led_toggle(i);                          // a changer apres le test que ca fonctionne bien
//        ++i;
//    }
//}