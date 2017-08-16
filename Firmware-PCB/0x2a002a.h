#ifndef OX2A002A_H
#define	OX2A002A_H

typedef signed char		s8;
typedef signed short	s16;
typedef signed long		s32;
typedef unsigned char	u8;
typedef unsigned short	u16;
typedef unsigned long	u32;
typedef void			(* generic_callback)(void);

#define FALSE	0
#define TRUE	!FALSE

# include "UART.h"
# include "I2C.h"
# include "SPI.h"

# include "HT16.h"
# include "ENCODERS.h"
# include "LCD.h"
# include "EEPROM.h"
# include "SDCARD.h"

# include "MIDI.h"
# include <stdlib.h>
# include <string.h>

//extern u16 __g_qbeat_pr;

//#define GET_BPM() ((((float)FREQUENCY / 256) * 15) / __g_qbeat_pr)
////#define GET_BPM() ((((float)FREQUENCY / (float)__g_qbeat_pr) * 15) / 256)
#define IS_NOTE_ATTACK(x)		(!(IS_NOTE_CONTINUOUS(x)))
#define IS_NOTE_CONTINUOUS(x)	(x & E_NOTE_CONTINUOUS)
#define NOTE_VALUE(x)			(x & 0x7F)

#define FREQUENCY				(8000000ul)
#define ONE_MILLISECOND			FREQUENCY / 1000
#define BUTTON_POLL_DELAY_MS	50
#define BUTTON_POLLS_PER_SECOND	20
#define SCREEN_DURATION_MS		1000
#define INITIAL_BPM_x100		14200
#define CLEAR_WATCHDOG			WDTCONbits.WDTCLR = 1

#define INSTRUMENTS_COUNT		16
#define PATTERNS_PER_INSTRUMENT	16
#define QTIME_PER_PATTERN		16
#define NOTES_PER_QTIME			8
#define ATTRIBUTES_PER_NOTE		2
#define MUSIC_PLAYING			1
#define MUSIC_PAUSE				0
#define PIANO_KEYS				0b1111111101110110
#define NO_NOTE					0xFF

// CONSTANTS
#define DEFAULT_BPM				142
#define GPIO_INPUT				1
#define GPIO_OUTPUT				0
#define RISING_EDGE				1
#define FALLING_EDGE			0
#define FLAG_UP					1
#define FLAG_DOWN				0
#define INT_ENABLED				1
#define INT_DISABLED			0
#define CS_LINE_UP				1
#define CS_LINE_DOWN 			0
#define DIGITAL_PIN				0
#define ANALOG_PIN				1


//PERIPHERAL PIN SELECT
#define PPS_COD_A				INT2R = 0b0100
#define COD_A_ANALOG			ANSELBbits.ANSB2
#define PPS_COD_B				INT4R = 0b0001
#define COD_B_ANALOG			ANSELBbits.ANSB3
#define PPS_SDI1				SDI1R = 0b0000
#define SDI1_ANALOG				ANSELAbits.ANSA1
#define PPS_SDO1				RPB13R = 0b0011
#define SDO1_ANALOG				ANSELBbits.ANSB13
#define SPI1_CS2_ANALOG			ANSELBbits.ANSB12
#define SPI1_CS3_ANALOG			ANSELBbits.ANSB15
#define PPS_MIDI				RPA0R = 0b0001
#define MIDI_ANALOG				ANSELAbits.ANSA0
#define PPS_MCP_ENC_A			INT1R = 0b0010
#define MCP_ENC_A_ANALOG		ANSELBbits.ANSB0
#define PPS_MCP_ENC_B			INT3R = 0b0010
#define MCP_ENC_B_ANALOG		ANSELBbits.ANSB1

//TIMERS
#define TIMER_A_PRESCALE_64		0b10
#define TIMER_A_PRESCALE_256	0b11
#define TIMER_B_PRESCALE_1		0b000
#define TIMER_B_PRESCALE_2		0b001
#define TIMER_B_PRESCALE_4		0b010
#define TIMER_B_PRESCALE_8		0b011
#define TIMER_B_PRESCALE_16		0b100
#define TIMER_B_PRESCALE_32		0b101
#define TIMER_B_PRESCALE_64		0b110
#define TIMER_B_PRESCALE_256	0b111

// timer 1 used for long press management
#define TIMER1_STOP_AND_RESET	T1CON = 0
#define TIMER1_VALUE			TMR1
#define TIMER1_PRESCALE			T1CONbits.TCKPS
#define TIMER1_ON				T1CONbits.ON = 1
#define LONG_PRESS_DONE			0xFFFFFFFF

// timer 2 used for pattern management and blinking during pause
#define TIMER2_STOP_AND_RESET	T2CON = 0
#define TIMER2_VALUE			TMR2
#define TIMER2_PERIOD			PR2
#define TIMER2_PRESCALE			T2CONbits.TCKPS
#define TIMER2_INT_FLAG_CLR		IFS0CLR = (1 << 9)
#define TIMER2_INT_PRIORITY		IPC2bits.T2IP
#define TIMER2_INT_ENABLE		IEC0bits.T2IE
#define TIMER2_ON				T2CONbits.ON = 1

// timer 3 used for bpm button
#define TIMER3_STOP_AND_RESET	T3CON = 0
#define TIMER3_VALUE			TMR3
#define TIMER3_PERIOD			PR3
#define TIMER3_PRESCALE			T3CONbits.TCKPS
#define TIMER3_INT_FLAG_CLR		IFS0CLR = (1 << 14)
#define TIMER3_INT_PRIORITY		IPC3bits.T3IP
#define TIMER3_INT_ENABLE		IEC0bits.T3IE

// timer 4 used for callback timer : duration of templates, EEPROM write etc
#define TIMER4_BUF_SIZE			10
#define TIMER4_STOP_AND_RESET	T4CON = 0
#define TIMER4_VALUE			TMR4
#define TIMER4_PERIOD			PR4
#define TIMER4_PRESCALE			T4CONbits.TCKPS
#define TIMER4_INT_FLAG_CLR		IFS0CLR = (1 << 19)
#define TIMER4_INT_PRIORITY		IPC4bits.T4IP
#define TIMER4_INT_ENABLE		IEC0bits.T4IE
#define TIMER4_IS_ON			T4CONbits.ON == 1
#define TIMER4_ON				T4CONbits.ON = 1
#define TIMER4_OFF				T4CONbits.ON = 0

// timer 5 used for key press management
#define TIMER5_STOP_AND_RESET	T5CON = 0
#define TIMER5_VALUE			TMR5
#define TIMER5_PERIOD			PR5
#define TIMER5_PRESCALE			T5CONbits.TCKPS
#define TIMER5_INT_FLAG_CLR		IFS0CLR = (1 << 24)
#define TIMER5_INT_PRIORITY		IPC5bits.T5IP
#define TIMER5_INT_ENABLE		IEC0bits.T5IE
#define TIMER5_ON				T5CONbits.ON = 1
#define TIMER5_OFF				T5CONbits.ON = 0


enum	E_NOTE_ATTRS
{
	E_NOTE_VALUE = 0,
	E_NOTE_VELOCITY,
};

enum	E_NOTE_MODS
{
	E_NOTE_ATTACK = 0x00,
	E_NOTE_CONTINUOUS = 0x80,
};

enum E_EVENT_SOURCE // dans l'ordre de mapping des boutons, ne pas changer
{
	E_SOURCE_KEY_0,
	E_SOURCE_KEY_1,
	E_SOURCE_KEY_2,
	E_SOURCE_KEY_3,
	E_SOURCE_KEY_4,
	E_SOURCE_KEY_5,
	E_SOURCE_KEY_6,
	E_SOURCE_KEY_7,
	E_SOURCE_KEY_8,
	E_SOURCE_KEY_9,
	E_SOURCE_KEY_10,
	E_SOURCE_KEY_11,
	E_SOURCE_KEY_12,
	E_SOURCE_KEY_13,
	E_SOURCE_KEY_14,
	E_SOURCE_KEY_15,

	E_SOURCE_BUTTON_PLAY_PAUSE,
	E_SOURCE_BUTTON_CUE,
	E_SOURCE_BUTTON_REC,
	E_SOURCE_BUTTON_TAP,

	E_SOURCE_BUTTON_EDIT,
	E_SOURCE_BUTTON_INSTRUMENT,
	E_SOURCE_BUTTON_PATTERN,
	E_SOURCE_BUTTON_KEYBOARD,

	E_SOURCE_ENCODER_0,
	E_SOURCE_ENCODER_1,
	E_SOURCE_ENCODER_2,
	E_SOURCE_ENCODER_3,
	E_SOURCE_ENCODER_4,
	E_SOURCE_ENCODER_5,
	E_SOURCE_ENCODER_6,
	E_SOURCE_ENCODER_7,

	E_SOURCE_ENCODER_MAIN
};

enum	E_EVENT_TYPE
{
	E_EVENT_NONE = 0,
	E_KEY_PRESSED,
	E_KEY_RELEASED,
	E_KEY_LONG_PRESSED,
	E_KEY_COMBINATION_PRESSED,
	E_ENCODER_TURNED_RIGHT,
	E_ENCODER_TURNED_LEFT,
	E_ENCODER_NO_DIRECTION
};


enum	E_MODES
{
	E_MODE_EDIT_PATTERN,
	E_MODE_PATTERN,
	E_MODE_EDIT_INSTRU,
	E_MODE_INSTRU,
	E_MODE_EDIT_KEYBOARD,
	E_MODE_KEYBOARD
};

#endif	/* OX2A002A_H */
