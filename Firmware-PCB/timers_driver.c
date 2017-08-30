#include <xc.h>
#include <sys/attribs.h>
#include "0x2a002a.h"

static const u16		timer4_prescale = 256;
static generic_callback	timer4_callbacks[TIMER4_BUF_SIZE] = { NULL };
static u16				timer4_periods[TIMER4_BUF_SIZE] = { 0 };
static u8				timer4_count = 0;
static u8				timer4_index = 0;

void	timer4_start(void)
{
	TIMER4_INT_FLAG_CLR;
	TIMER4_PERIOD = timer4_periods[timer4_index];
	TIMER4_VALUE = 0;
	TIMER4_ON;
}

void	timer4_restart(u16 period)
{
	TIMER4_PERIOD = period;
	TIMER4_VALUE = 0;
	TIMER4_ON;
}

void	int_init_timer4(void)
{
	TIMER4_INT_FLAG_CLR;
	TIMER4_INT_PRIORITY = 1;
	TIMER4_INT_ENABLE = INT_ENABLED;
}

void	timer_4_init(void)
{
	TIMER4_STOP_AND_RESET;
	TIMER4_PRESCALE = TIMER_B_PRESCALE_256;
}

void	__ISR(_TIMER_4_VECTOR, IPL1AUTO) Timer4Handler(void)
{
	generic_callback cb = timer4_callbacks[timer4_index];

	TIMER4_INT_FLAG_CLR;
	if (++timer4_index == timer4_count)
	{
		TIMER4_OFF;
		timer4_index = 0;
		timer4_count = 0;
	}
	else
	{
		TIMER4_PERIOD = timer4_periods[timer4_index];
		TIMER4_VALUE = 0;
	}
	(*cb)();
}

void	timer4_push(u16 period_ms, generic_callback cb)
{
	if (TIMER4_IS_ON && timer4_callbacks[timer4_index] == cb)
		timer4_restart(ONE_MILLISECOND * period_ms / timer4_prescale - 1);
	else
	{
		TIMER4_INT_ENABLE = 0;
		timer4_callbacks[timer4_count] = cb;
		timer4_periods[timer4_count++] = ONE_MILLISECOND * period_ms / timer4_prescale - 1;
		TIMER4_INT_ENABLE = 1;
		if (timer4_count == 1)
			timer4_start();
	}
}
