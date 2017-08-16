#include <xc.h>
#include <sys/attribs.h>
#include "0x2a002a.h"

u8					UART1_buf[0x100];
u8					UART1_buf_index = 0;
u8					UART1_buf_len = 0;
generic_callback	UART1_idle_callback = NULL;

u8	UART1_is_idle(void)
{
	return (UART1_buf_len == 0);
}

void	UART1_init(void)
{
	U1MODE = 0;
	U1MODEbits.ON = 0;
	U1MODEbits.BRGH = 0;
	U1BRG = SET_MIDI_BRG;
	U1MODEbits.RTSMD = 1;
	U1MODEbits.UEN = 0b00;
	U1STAbits.UTXEN = 1;
	U1STAbits.URXEN = 0;
	U1MODEbits.ON = 1;
}

void	pps_init_UART(void)
{
	PPS_MIDI;
	COD_MCP_A_GPIO = GPIO_INPUT;
}

void	int_init_UART(void)
{
	UART1_TX_INT_FLAG_CLR;
	UART1_TX_INT_PRIORITY = 5;
	// ~UART1_TX_INT_ENABLE = INT_ENABLED;~ UART interrupt is enabled on sent messages
}

void	__ISR(_UART_1_VECTOR, IPL5AUTO) UART1_TX_handler(void)
{
	if (UART1_buf_len == 0)
	{
		UART1_TX_INT_ENABLE = INT_DISABLED;
		if (UART1_idle_callback)
			UART1_idle_callback();
	}
	else
	{
		U1TXREG = UART1_buf[UART1_buf_index++];
		UART1_buf_len--;
	}
	UART1_TX_INT_FLAG_CLR;
}

void	UART1_push(u8 buffer[], u8 size)
{
	u8	i = 0;

	__builtin_disable_interrupts();
	while (i < size)
		UART1_buf[(UART1_buf_index + UART1_buf_len++) & 0xFF] = buffer[i++];
	UART1_TX_INT_ENABLE = INT_ENABLED;
	__builtin_enable_interrupts();
}
