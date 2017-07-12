#include <xc.h>
#include <sys/attribs.h>
#include "0x2a002a.h"
#include <stdlib.h>

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
extern const u8 *notesnames[];
extern u8	lcd_chars[8][21];
u8              cur_template = TEMPLATE_DEFAULT;
void    request_template(u8 template)
{
    cur_template = template;
    LCD_dirty = 1;
}

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
    TIMER4_PRESCALE = TIMER_B_PRESCALE_256;
    TIMER4_PERIOD = 60000;
//	TIMER4_PERIOD = ONE_MILLISECOND / 64 * SCREEN_DURATION_MS - 1;
}

void __ISR(_TIMER_4_VECTOR, IPL1AUTO) Timer4Handler(void)
{
        request_template(TEMPLATE_DEFAULT);
	IFS0bits.T4IF = 0;
	T4CONbits.ON = 0;
}

u8      *negate_string(u8 *str, u8 *dest)
{
    u8  i;

    i = 0;
    while (dest[i])
        dest[i++] = 0;
    i = 0;
    while (str[i])
    {
        dest[i] = str[i] + 0x80;
        i++;
    }
    return (dest);
}

void    display_current_template(void)
{
    static u8 previous = TEMPLATE_DEFAULT;
    static u8 last_encoder = 0;
    u8  lines[LINES_COUNT][CHARS_PER_LINE + 1] = { 0 };
    u8  i = 0;
    u8  s1[CHARS_PER_LINE + 1];
    u8  s2[CHARS_PER_LINE + 1];

    if (cur_template == TEMPLATE_ENCODER && cur_template == previous)
    {
        snprintf(s1, CHARS_PER_LINE + 1, "%3d", encoders_values[cur_encoder] / 2);
        LCD_print_char(3, 11, cur_encoder + '1' + 0x80);
        lcd_chars[3][11] = cur_encoder + '1' + 0x80;
        LCD_print_char(4, 7, s1[0]);
        lcd_chars[4][7] = s1[0];
        LCD_print_char(4, 8, s1[1]);
        lcd_chars[4][8] = s1[1];
        LCD_print_char(4, 9, s1[2]);
        lcd_chars[4][9] = s1[2];
        last_encoder = cur_encoder;
    }
    else
    {
        LCD_clear();
        switch (cur_template)
        {
            case TEMPLATE_DEFAULT:
                snprintf(lines[0], CHARS_PER_LINE + 1, "%10s%10s", negate_string("INSTRUMENT", s1), negate_string("PATTERN", s2));
                snprintf(lines[1], CHARS_PER_LINE + 1, "%10d%10d", cur_instrument + 1, cur_pattern + 1);
                snprintf(lines[3], CHARS_PER_LINE + 1, "%10s%10s", negate_string("NOTE", s1), negate_string("VELOCITY", s2));
                snprintf(lines[4], CHARS_PER_LINE + 1, "%10s%10d", notesnames[cur_note % 12], cur_velocity);
                snprintf(lines[6], CHARS_PER_LINE + 1, "%10s%10s", negate_string("BPM", s1), negate_string("OCTAVE", s2));
                snprintf(lines[7], CHARS_PER_LINE + 1, "%10.2f%10d", ((float)bpm_x100 / 100.0), cur_octave + 1);
                break;
            case TEMPLATE_BPM:
                snprintf(s1, CHARS_PER_LINE + 1, "%10s%20c", "BPM", 0);
                snprintf(lines[3], CHARS_PER_LINE + 1, "%10s", negate_string(s1, s2));
                snprintf(lines[4], CHARS_PER_LINE + 1, "%10.2f", ((float)bpm_x100 / 100.0));
                break;
            case TEMPLATE_ENCODER:
                snprintf(s1, CHARS_PER_LINE + 1, "%10s %d%20c", "ENCODER", cur_encoder + 1, 0);
                snprintf(lines[3], CHARS_PER_LINE + 1, "%10s", negate_string(s1, s2));
                snprintf(lines[4], CHARS_PER_LINE + 1, "%10d", encoders_values[cur_encoder] / 2);
                break;
            case TEMPLATE_MAIN_MENU_START:
                break;
            case TEMPLATE_NOTE:
                snprintf(s1, CHARS_PER_LINE + 1, "%10s%20c", "NOTE", 0);
                snprintf(lines[3], CHARS_PER_LINE + 1, "%10s", negate_string(s1, s2));
                snprintf(lines[4], CHARS_PER_LINE + 1, "%10s", notesnames[cur_note % 12]);
                break;
            case TEMPLATE_VELOCITY:
                snprintf(s1, CHARS_PER_LINE + 1, "%10s%20c", "VELOCITY", 0);
                snprintf(lines[3], CHARS_PER_LINE + 1, "%10s", negate_string(s1, s2));
                snprintf(lines[4], CHARS_PER_LINE + 1, "%10d", cur_velocity);
                break;
            case TEMPLATE_OCTAVE:
                snprintf(s1, CHARS_PER_LINE + 1, "%10s%20c", "OCTAVE", 0);
                snprintf(lines[3], CHARS_PER_LINE + 1, "%10s", negate_string(s1, s2));
                snprintf(lines[4], CHARS_PER_LINE + 1, "%10d", cur_octave + 1);
                break;
            case TEMPLATE_INSTRUMENT:
                snprintf(s1, CHARS_PER_LINE + 1, "%10s%20c", "INSTRUMENT", 0);
                snprintf(lines[3], CHARS_PER_LINE + 1, "%10s", negate_string(s1, s2));
                snprintf(lines[4], CHARS_PER_LINE + 1, "%10d", cur_instrument + 1);
                break ;
            case TEMPLATE_PATTERN:
                snprintf(s1, CHARS_PER_LINE + 1, "%10s%20c", "PATTERN", 0);
                snprintf(lines[3], CHARS_PER_LINE + 1, "%10s", negate_string(s1, s2));
                snprintf(lines[4], CHARS_PER_LINE + 1, "%10d", cur_pattern + 1);
                break ;
            case TEMPLATE_PATTERN_RECORDED:
                snprintf(s1, CHARS_PER_LINE + 1, "%10s%20c", "PATTERN RECORDED", 0);
                snprintf(lines[3], CHARS_PER_LINE + 1, "%10s", negate_string(s1, s2));
                break ;

        }
        while (i < LINES_COUNT)
        {
            LCD_putstr(i, 0, lines[i]);
            ++i;
        }
        LCD_print_changed_chars();
    }
    if (cur_template != TEMPLATE_DEFAULT)
    {
        TMR4 = 0;
        T4CONbits.ON = 1;
    }
    previous = cur_template;
}