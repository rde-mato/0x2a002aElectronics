#include <xc.h>
#include <sys/attribs.h>
#include "0x2a002a.h"
#include "charset.h"

void __ISR(_TIMER_4_VECTOR, IPL1AUTO) Timer5Handler(void)
{
    IFS0bits.T4IF = 0;
    T4CONbits.ON = 0;
	template_default();
}

void	template_default(void)
{
}

void	template_encoder(u8 encoder, u8 value)
{
}

void	template_main_menu(u8 position)
{
	LCD_putstr(3, 0, "  TEST  ");
	T1CONbits.ON = 1;
}

void	template_bpm(void)
{
}

void	template_bpm(void)
{
}
