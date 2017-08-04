#ifndef SIMULATION_H
# define SIMULATION_H

# define CLEAR_WATCHDOG          WDTCONbits.WDTCLR = 1
# define __ISR(x, y) 

//TIMERS
# define TIMER_A_PRESCALE_64       0b10
# define TIMER_A_PRESCALE_256      0b11
# define TIMER_B_PRESCALE_1        0b000
# define TIMER_B_PRESCALE_2        0b001
# define TIMER_B_PRESCALE_4        0b010
# define TIMER_B_PRESCALE_8        0b011
# define TIMER_B_PRESCALE_16       0b100
# define TIMER_B_PRESCALE_32       0b101
# define TIMER_B_PRESCALE_64       0b110
# define TIMER_B_PRESCALE_256      0b111

// timer 1 used for long press management
# define TIMER1_STOP_AND_RESET	T1CON = 0
# define TIMER1_SET_VALUE(x)    TMR1 = x
# define TIMER1_GET_VALUE	      TMR1
# define TIMER1_GET_PRESCALE    T1CONbits.TCKPS
# define TIMER1_SET_PRESCALE(x) T1CONbits.TCKPS = x
# define TIMER1_ON              T1CONbits.ON = 1
# define LONG_PRESS_DONE         0xFFFFFFFF

// timer 2 used for pattern management and blinking during pause
# define TIMER2_STOP_AND_RESET	T2CON = 0
# define TIMER2_SET_VALUE(x)		TMR2 = x
# define TIMER2_GET_VALUE		    TMR2
# define TIMER2_SET_PERIOD(x)   PR2 = x
# define TIMER2_GET_PERIOD      PR2
# define TIMER2_SET_PRESCALE(x) T2CONbits.TCKPS = x
# define TIMER2_GET_PRESCALE    T2CONbits.TCKPS
# define TIMER2_INT_FLAG_CLR    IFS0CLR = (1 << 9)
# define TIMER2_INT_PRIORITY    IPC2bits.T2IP
# define TIMER2_SET_INT_ENABLE(x)	IEC0bits.T2IE = x
# define TIMER2_ON              T2CONbits.ON = 1

// timer 3 used for bpm button
# define TIMER3_STOP_AND_RESET   T3CON = 0
# define TIMER3_SET_VALUE(x)     TMR3
# define TIMER3_GET_VALUE        TMR3
# define TIMER3_SET_PERIOD(x)    PR3
# define TIMER3_GET_PERIOD       PR3
# define TIMER3_SET_PRESCALE(x)  T3CONbits.TCKPS
# define TIMER3_GET_PRESCALE     T3CONbits.TCKPS
# define TIMER3_INT_FLAG_CLR     IFS0CLR = (1 << 14)
# define TIMER3_INT_PRIORITY     IPC3bits.T3IP
# define TIMER3_SET_INT_ENABLE(x) IEC0bits.T3IE = x
# define TIMER3_IS_ON            T3CONbits.ON
# define TIMER3_ON               T3CONbits.ON = 1
# define TIMER3_OFF              T3CONbits.ON = 0

// timer 4 used for callback timer : duration of templates, EEPROM write etc
# define TIMER4_BUF_SIZE         10
# define TIMER4_STOP_AND_RESET	 T4CON = 0
# define TIMER4_SET_VALUE(x)	 	 TMR4
# define TIMER4_GET_VALUE		     TMR4
# define TIMER4_SET_PERIOD(x)	 	 PR4
# define TIMER4_GET_PERIOD		   PR4
# define TIMER4_SET_PRESCALE(x)	 T4CONbits.TCKPS
# define TIMER4_GET_PRESCALE		 T4CONbits.TCKPS
# define TIMER4_INT_FLAG_CLR	   IFS0CLR = (1 << 19)
# define TIMER4_INT_PRIORITY	   IPC4bits.T4IP
# define TIMER4_SET_INT_ENABLE(x)	IEC0bits.T4IE = x
# define TIMER4_IS_ON            T4CONbits.ON == 1
# define TIMER4_ON               T4CONbits.ON = 1
# define TIMER4_OFF              T4CONbits.ON = 0

// timer 5 used for key press management
# define TIMER5_STOP_AND_RESET	 T5CON = 0
# define TIMER5_SET_VALUE(x)		 TMR5 = x
# define TIMER5_GET_VALUE		     TMR5
# define TIMER5_SET_PERIOD(x)		 PR5 = x
# define TIMER5_PERIOD		       PR5
# define TIMER5_SET_PRESCALE(x)  T5CONbits.TCKPS = x
# define TIMER5_GET_PRESCALE		 T5CONbits.TCKPS
# define TIMER5_INT_FLAG_CLR	   IFS0CLR = (1 << 24)
# define TIMER5_INT_PRIORITY	   IPC5bits.T5IP
# define TIMER5_SET_INT_ENABLE(x)	IEC0bits.T5IE = x
# define TIMER5_ON               T5CONbits.ON = 1
# define TIMER5_OFF              T5CONbits.ON = 0

// ENCODERS_H

// HT16_H
# define HT16_SET_INT_POLARITY(x)	INTCONbits.INT0EP = x
# define HT16_GET_INT_POLARITY	INTCONbits.INT0EP
# define HT16_SET_INT_FLAG(x)		IFS0bits.INT0IF = x
# define HT16_GET_INT_FLAG		  IFS0bits.INT0IF
# define HT16_INT_FLAG_CLR	    IFS0CLR = (1 << 3)
# define HT16_INT_PRIORITY	    IPC0bits.INT0IP
# define HT16_SET_INT_ENABLE		IEC0bits.INT0IE

// I2C_H
# define I2C1_READY		(I2C1_state == E_I2C1_DONE)
# define I2C1_INT_FLAG_CLR	IFS1CLR = (1 << 12)
# define I2C1_SET_PIN_GPIO(x)		TRISBbits.TRISB8 = x
# define I2C1_GET_PIN_GPIO		TRISBbits.TRISB8
# define I2C1_SET_PIN_LATCH(x) 		LATBbits.LATB8 = x
# define I2C1_GET_PIN_LATCH 		LATBbits.LATB8
# define I2C1_INT_PRIORITY	IPC8bits.I2C1IP
# define I2C1_SET_INT_ENABLE(x)		IEC1bits.I2C1MIE = x

# define I2C1_RW_READ		0
# define I2C1_RW_WRITE		1
# define I2C1_ACK		0
# define I2C1_NACK		1

# define I2C1_WRITE_BUF_SIZE     10
# define I2C1_READ_BUF_SIZE      10

#endif
