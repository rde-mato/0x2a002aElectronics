#include <xc.h>
#include <sys/attribs.h>
#include "0x2a002a.h"
#include <stdlib.h>

#define NEG_SPACE 160 //' ' + 0x80
extern u32      bpm_x100;
extern u8	cur_instrument;
extern u8	cur_pattern;
extern u8	cur_note;
extern u8	cur_octave;
extern u8	cur_velocity;
extern u8       LCD_dirty;
extern u8       cur_encoder;
extern u8       encoders_values[];
extern u8       encoders_scale[];
extern const u8    *notesnames[];
lcd_template    requested_template = &template_default;
/*
** we can't use requested_template as last_template because request_template
** can be called multiple times before actually displaying the template.
*/
u8              last_template = NO_TEMPLATE;

void    int_init_timer4(void)
{
    TIMER4_INT_FLAG_CLR;
    TIMER4_INT_PRIORITY = 1;
    TIMER4_INT_ENABLE = INT_ENABLED;
}

void    timer_4_init(void)
{
    TIMER4_STOP_AND_RESET;
    TIMER4_VALUE = 0;
    TIMER4_PRESCALE = TIMER_PRESCALE_256;
    TIMER4_PERIOD = 60000;
//	TIMER4_PERIOD = ONE_MILLISECOND / 64 * SCREEN_DURATION_MS - 1;
}

void __ISR(_TIMER_4_VECTOR, IPL1AUTO) Timer4Handler(void)
{
        request_template(&template_default);
	IFS0bits.T4IF = 0;
	T4CONbits.ON = 0;
}

void    request_template(lcd_template template)
{
    LCD_dirty = 1;
    requested_template = template;
//    if (template != &template_default)  Handled by templates themselves ?
//    {
//        TMR4 = 0;
//        T4CONbits.ON = 1;
//    }
}

void    negate_spaces(u8 *str)
{
    while (*str)
    {
        if (*str == ' ')
            *str += 0x80;
        str++;
    }
}

void		template_default(void)
{
    u8 line0[LINE_MAX_LEN] = "INSTRUMENT  PATTERN ";
    u8 line3[LINE_MAX_LEN] = "NOTE     VELOCITY";
    u8 line6[LINE_MAX_LEN] = "BPM       OCTAVE";
    u8 buf[LINE_MAX_LEN] = {0};

    negate_spaces(line0);
    negate_spaces(line3);
    negate_spaces(line6);

    LCD_clear();
    LCD_putstr_negative(0, 0, line0);
    snprintf(buf, LINE_MAX_LEN, "%d          %d", cur_instrument + 1, cur_pattern + 1);
    LCD_putstr(1, 4, buf);
    LCD_putstr_negative(3, 3, line3);
    snprintf(buf, LINE_MAX_LEN, "%-2s         %d", notesnames[cur_note % 12], cur_velocity);
    LCD_putstr(4, 4, buf);
    LCD_putstr_negative(6, 3, line6);
    snprintf(buf, LINE_MAX_LEN, "%.2f        %d", ((float)bpm_x100 / 100.0), cur_octave + 1);
    LCD_putstr(7, 2, buf);
    LCD_print_changed_chars();
    last_template = TEMPLATE_DEFAULT;
}

void		template_encoder(void)
{
    static u8   last_encoder = 0xFF;
    u8          buf[LINE_MAX_LEN] = {0};

    if (last_template != TEMPLATE_ENCODER)
        LCD_clear();
    if ((last_encoder != cur_encoder) || last_template != TEMPLATE_ENCODER)
    {
        snprintf(buf, LINE_MAX_LEN, "      ENCODER %d      ", cur_encoder + 1);
        LCD_putstr_negative(1, 0, buf);
        last_encoder = cur_encoder;
    }
    snprintf(buf, LINE_MAX_LEN, "%-3d", encoders_values[cur_encoder] / 2);
    LCD_putstr(3, 3, buf);
    LCD_print_changed_chars();
    TMR4 = 0;
    T4CONbits.ON = 1;
    last_template = TEMPLATE_ENCODER;
}

void		template_main_menu_start(void)
{
    LCD_clear();
    LCD_putstr(0, 0, "   TEST");
    LCD_print_changed_chars();
    TMR4 = 0;
    T4CONbits.ON = 1;
    last_template = TEMPLATE_MAIN_MENU_START;
}

void		template_bpm(void)
{
    u8          line2[LINE_MAX_LEN] = "         BPM         ";
    u8          line4[LINE_MAX_LEN] = { 0 };
    extern u8   tap_pressed;

    if (last_template != TEMPLATE_BPM)
    {
        LCD_clear();
        LCD_putstr_negative(3, 0, line2);
    }
    snprintf(line4, LINE_MAX_LEN, "%-7.2f", ((float)bpm_x100 / 100.0)); // change padding to 6 if bpm restricted < 1000
    LCD_putstr(4, 8, line4);
    LCD_print_changed_chars();
    if (!tap_pressed)
    {
        TMR4 = 0;
        T4CONbits.ON = 1;
    }
    last_template = TEMPLATE_BPM;
}

void		template_note(void)
{
    u8 line2[LINE_MAX_LEN] = "        NOTE         ";
    u8 line4[LINE_MAX_LEN] = { 0 };

    if (last_template != TEMPLATE_NOTE)
    {
        LCD_clear();
        LCD_putstr_negative(3, 0, line2);
    }
    snprintf(line4, LINE_MAX_LEN, "%-2s", notesnames[cur_note % 12]);
    LCD_putstr(4, 8, line4);
    LCD_print_changed_chars();
    TMR4 = 0;
    T4CONbits.ON = 1;
    last_template = TEMPLATE_NOTE;
}

void		template_octave(void)
{
    u8 line2[LINE_MAX_LEN] = "        OCTAVE       ";
    u8 line4[LINE_MAX_LEN] = { 0 };

    if (last_template != TEMPLATE_NOTE)
    {
        LCD_clear();
        LCD_putstr_negative(3, 0, line2);
    }
    snprintf(line4, LINE_MAX_LEN, "%d", cur_octave + 1);
    LCD_putstr(4, 8, line4);
    LCD_print_changed_chars();
    TMR4 = 0;
    T4CONbits.ON = 1;
    last_template = TEMPLATE_NOTE;
}

void		template_velocity(void)
{
    u8 line2[LINE_MAX_LEN] = "      VELOCITY       ";
    u8 line4[LINE_MAX_LEN] = { 0 };

    if (last_template != TEMPLATE_VELOCITY)
    {
        LCD_clear();
        LCD_putstr_negative(3, 0, line2);
    }
    snprintf(line4, LINE_MAX_LEN, "%-3d", cur_velocity);
    LCD_putstr(4, 8, line4);
    LCD_print_changed_chars();
    TMR4 = 0;
    T4CONbits.ON = 1;
    last_template = TEMPLATE_VELOCITY;
}