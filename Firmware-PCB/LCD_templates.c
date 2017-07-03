#include <xc.h>
#include <sys/attribs.h>
#include "0x2a002a.h"

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