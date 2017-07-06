#ifndef UART_H
#define	UART_H

#define UART1_TX_INT_ENABLE     IEC1bits.U1TXIE
#define UART1_TX_INT_PRIORITY   IPC8bits.U1IP
#define UART1_TX_INT_FLAG       IFS1bits.U1TXIF
#define UART1_TX_INT_FLAG_CLR   IFS1CLR = (1 << 8)

#endif	/* UART_H */

