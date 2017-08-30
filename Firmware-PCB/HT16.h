#ifndef HT16_H
#define HT16_H

#include "0x2a002a.h"


#define HT16_INT_POLARITY	INTCONbits.INT0EP
#define HT16_INT_FLAG		IFS0bits.INT0IF
#define HT16_INT_FLAG_CLR	IFS0CLR = (1 << 3)
#define HT16_INT_PRIORITY	IPC0bits.INT0IP
#define HT16_INT_ENABLE		IEC0bits.INT0IE

#define		TAP_PRESSED		(all.current_key_scan & (1 << E_SOURCE_BUTTON_TAP))
#define		EDIT_PRESSED	(all.current_key_scan & (1 << E_SOURCE_BUTTON_EDIT))
#define		CUE_PRESSED		(all.current_key_scan & (1 << E_SOURCE_BUTTON_CUE))

typedef void	(* read_callback)(unsigned char *);
typedef void	(* write_callback)(void);

#endif/* HT16_H */
