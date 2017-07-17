#ifndef UART_H
#define	UART_H

#define SET_MIDI_BRG            ((u16)(PBCLK / (16 * MIDI_BAUD_RATE) - 1))
#define PBCLK                   8000000
#define MIDI_BAUD_RATE          31250
#define TIMING_CLOCK            0xF8

#define UART1_TX_INT_FLAG_CLR   IFS1CLR = (1 << 8)
#define UART1_TX_INT_ENABLE     IEC1bits.U1TXIE
#define UART1_TX_INT_PRIORITY   IPC8bits.U1IP
#define UART1_TX_INT_FLAG       IFS1bits.U1TXIF

#endif	/* UART_H */