#include <xc.h>
#include <sys/attribs.h>
#include "0x2a002a.h"

void lcditoa(u32 value, u8 *res, u8 nbsize)
{
    u8 i = 0;

    while (i < nbsize)
    {
        if (!(value % 10))
            res[i] = ' ';
        else
            res[i] = value % 10 + '0';
        value /= 10;
        i++;
    }
    res[nbsize] = '\0';
}


void	template_default(void)
{
    LCD_clear();
	LCD_putstr(0, 0, "  DEFAULT  ");
        LCD_print_changed_chars();
}

void __ISR(_TIMER_4_VECTOR, IPL1AUTO) Timer4Handler(void)
{
    IFS0bits.T4IF = 0;
	T4CONbits.ON = 0;
	template_default();
}

void	template_encoder(u8 encoder, u8 value)
{
}

void	template_main_menu(u8 position)
{
    LCD_clear();
	LCD_putstr(0, 0, "   TEST");
        LCD_print_changed_chars();
	T4CONbits.ON = 1;
}

void	template_bpm(void)
{
}