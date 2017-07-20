#include <xc.h>
#include <sys/attribs.h>
#include "0x2a002a.h"

u8  UART1_buf[0x100];
u8  UART1_buf_index = 0;
u8  UART1_buf_len = 0;

void UART1_init(void)
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

void    pps_init_UART(void)
{
    MIDI_ANALOG = DIGITAL_PIN;
    PPS_MIDI;
    MCP_ENC_A_ANALOG = DIGITAL_PIN;
    COD_MCP_A_GPIO = GPIO_INPUT;
}

void    int_init_UART(void)
{
    UART1_TX_INT_FLAG_CLR;
    UART1_TX_INT_PRIORITY = 5;
    // ~UART1_TX_INT_ENABLE = INT_ENABLED;~ UART interrupt is enabled on sent messages
}

void __ISR(_UART_1_VECTOR, IPL5AUTO) UART1_TX_handler(void)
{
    U1TXREG = UART1_buf[UART1_buf_index++];
    if (--UART1_buf_len == 0)
       UART1_TX_INT_ENABLE = INT_DISABLED;
    UART1_TX_INT_FLAG_CLR;
}

void UART1_send_3(char byte1, char byte2, char byte3)
{
    __builtin_disable_interrupts();
    UART1_buf[(UART1_buf_index + UART1_buf_len++) & 0xFF] = byte1;
    UART1_buf[(UART1_buf_index + UART1_buf_len++) & 0xFF] = byte2;
    UART1_buf[(UART1_buf_index + UART1_buf_len++) & 0xFF] = byte3;
    UART1_TX_INT_ENABLE = INT_ENABLED;
    __builtin_enable_interrupts();
}