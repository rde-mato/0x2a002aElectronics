#include <xc.h>
#include <sys/attribs.h>
#include "0x2a002a.h"
#include <stdlib.h>

#define NEG_SPACE 160 //' ' + 0x80
extern u8	cur_instrument;
extern u8	cur_pattern;
extern u8	cur_note;
extern u8	cur_octave;
extern u8	cur_velocity;
extern float    g_bpm;

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
    snprintf(buf, LINE_MAX_LEN, "%d         %d", cur_note, cur_velocity);
    LCD_putstr(4, 4, buf);
    LCD_putstr_negative(6, 3, line6);
    snprintf(buf, LINE_MAX_LEN, "%.2f        %d", g_bpm, cur_octave);
    LCD_putstr(7, 2, buf);
    LCD_print_changed_chars();
}

void __ISR(_TIMER_4_VECTOR, IPL1AUTO) Timer4Handler(void)
{
	IFS0bits.T4IF = 0;
	T4CONbits.ON = 0;
	template_default();
}

void		template_encoder(u8 encoder, u8 value)
{
    u8 buf[LINE_MAX_LEN] = {0};

    LCD_clear();
    snprintf(buf, LINE_MAX_LEN, "ENCODER %d VALUE", encoder + 1);
    LCD_putstr_negative(0, 0, buf);
    snprintf(buf, LINE_MAX_LEN, "%d", value);
    LCD_putstr(3, 3, buf);
    LCD_print_changed_chars();
    TMR4 = 0;
    T4CONbits.ON = 1;
}

void		template_main_menu(u8 position)
{
    LCD_clear();
    LCD_putstr(0, 0, "   TEST");
    LCD_print_changed_chars();
    TMR4 = 0;
    T4CONbits.ON = 1;
}

void		template_bpm(void)
{
    u8 line2[LINE_MAX_LEN] = "         BPM         ";
    u8 line4[LINE_MAX_LEN] = { 0 };

    LCD_clear();
    LCD_putstr_negative(3, 0, line2);
    snprintf(line4, LINE_MAX_LEN, "%.2f", g_bpm);
    LCD_putstr(4, 8, line4);
    LCD_print_changed_chars();
    TMR4 = 0;
    T4CONbits.ON = 1;
}

void		template_note(void)
{
    u8 line2[LINE_MAX_LEN] = "        NOTE         ";
    u8 line4[LINE_MAX_LEN] = { 0 };

    LCD_clear();
    LCD_putstr_negative(3, 0, line2);
    snprintf(line4, LINE_MAX_LEN, "%d", cur_note);
    LCD_putstr(4, 8, line4);
    LCD_print_changed_chars();
    TMR4 = 0;
    T4CONbits.ON = 1;
}

void		template_velocity(void)
{
    u8 line2[LINE_MAX_LEN] = "      VELOCITY       ";
    u8 line4[LINE_MAX_LEN] = { 0 };

    LCD_clear();
    LCD_putstr_negative(3, 0, line2);
    snprintf(line4, LINE_MAX_LEN, "%d", cur_velocity);
    LCD_putstr(4, 8, line4);
    LCD_print_changed_chars();
    TMR4 = 0;
    T4CONbits.ON = 1;
}

//void		lcditoa(u32 value, u8 *res, u8 nbsize)
//{
//	u8		i = 0;
//
//	while (i < nbsize)
//	{
//		if (!(value % 10))
//			res[nbsize - i - 1] = '0';
//		else
//			res[nbsize - i - 1] = value % 10 + '0';
//		value /= 10;
//		i++;
//	}
//	res[nbsize] = '\0';
//}

//void		lcd_bzero(void *s, u32 n)
//{
//	u8		*ss;
//
//	ss = s;
//	while (n--)
//		*ss++ = 0;
//}

//void			lcd_strncpy(u8 *dest, const u8 *src, u32 n)
//{
//	u8		*d;
//	u8		*s;
//
//	d = dest;
//	s = (u8*)src;
//	while (n-- && *s)
//		*d++ = *s++;
//}