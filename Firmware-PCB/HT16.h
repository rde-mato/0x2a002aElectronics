#ifndef HT16_H
#define	HT16_H

#define HT16_INT_POLARITY	INTCONbits.INT0EP
#define HT16_INT_FLAG		IFS0bits.INT0IF
#define HT16_INT_FLAG_CLR	IFS0CLR = (1 << 3)
#define HT16_INT_PRIORITY	IPC0bits.INT0IP
#define HT16_INT_ENABLE		IEC0bits.INT0IE

typedef void                    (* read_callback)(unsigned char *);
typedef void                    (* write_callback)(void);

#endif	/* HT16_H */

