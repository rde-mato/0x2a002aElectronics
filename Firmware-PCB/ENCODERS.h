#ifndef ENCODERS_H
#define ENCODERS_H

//MAIN ENCODER
#define COD_A_GPIO				TRISBbits.TRISB2
#define COD_A_INT_POLARITY		INTCONbits.INT2EP
#define COD_A_INT_FLAG			IFS0bits.INT2IF
#define COD_A_INT_FLAG_CLR		IFS0CLR = (1 << 13)
#define COD_A_INT_PRIORITY		IPC2bits.INT2IP
#define COD_A_INT_ENABLE		IEC0bits.INT2IE
#define COD_B_GPIO				TRISBbits.TRISB3
#define COD_B_INT_POLARITY		INTCONbits.INT4EP
#define COD_B_INT_FLAG_CLR		IFS0CLR = (1 << 23)
#define COD_B_INT_FLAG			IFS0bits.INT4IF
#define COD_B_INT_PRIORITY		IPC4bits.INT4IP
#define COD_B_INT_ENABLE		IEC0bits.INT4IE

//MCP ENCODERS
#define COD_MCP_A_GPIO			TRISBbits.TRISB0
#define COD_MCP_A_INT_POLARITY	INTCONbits.INT1EP
#define COD_MCP_A_INT_FLAG		IFS0bits.INT1IF
#define COD_MCP_A_INT_FLAG_CLR	IFS0CLR = (1 << 8)
#define COD_MCP_A_INT_PRIORITY	IPC1bits.INT1IP
#define COD_MCP_A_INT_ENABLE	IEC0bits.INT1IE
#define COD_MCP_B_GPIO			TRISBbits.TRISB1
#define COD_MCP_B_INT_POLARITY	INTCONbits.INT3EP
#define COD_MCP_B_INT_FLAG		IFS0bits.INT3IF
#define COD_MCP_B_INT_FLAG_CLR	IFS0CLR = (1 << 18)
#define COD_MCP_B_INT_PRIORITY	IPC3bits.INT3IP
#define COD_MCP_B_INT_ENABLE	IEC0bits.INT3IE

#define MCP_ENC_READ_INT_FLAG	0x410E0000
#define MCP_ENC_READ_INT_CAP	0x41100000

#endif	/* ENCODERS_H */
