//-------------------------------------------------------------------------------------------------
// Universal KS0108 driver library
// Atmel AT91SAM7 MCU low-level driver
// (c) Rados³aw Kwiecieñ, radek@dxp.pl
//-------------------------------------------------------------------------------------------------
#include "build/AT91SAM7S256.h"

// data bus
#define KS0108_DATA_SET (*AT91C_PIOA_SODR)
#define KS0108_DATA_CLR (*AT91C_PIOA_CODR)
#define KS0108_DATA_OUT (*AT91C_PIOA_OER)
#define KS0108_DATA_INP (*AT91C_PIOA_ODR)
#define KS0108_DATA_PIN (*AT91C_PIOA_PDSR)
#define KS0108_DATA_IO_EN (*AT91C_PIOA_PER)

#define KS0108_D0		16
		
#define KS0108_CTRL_SET (*AT91C_PIOA_SODR)
#define KS0108_CTRL_CLR (*AT91C_PIOA_CODR)
#define KS0108_CTRL_OUT (*AT91C_PIOA_OER)
#define KS0108_CTRL_INP (*AT91C_PIOA_ODR)
#define KS0108_CTRL_PIN (*AT91C_PIOA_PDSR)
#define KS0108_CTRL_IO_EN (*AT91C_PIOA_PER)

#define KS0108_RS			(1 << 8)
#define KS0108_RW			(1 << 9)
#define KS0108_EN			(1 << 10)

#define KS0108_CS1			(1 << 12)
#define KS0108_CS2			(1 << 11)
#define KS0108_CS3			(1 << 13)

#define DISPLAY_STATUS_BUSY	0x80

extern unsigned char screen_x;
extern unsigned char screen_y;

extern void Init(void);

//-------------------------------------------------------------------------------------------------
// Delay function
//-------------------------------------------------------------------------------------------------
void GLCD_Delay(void)
{
asm("nop");
asm("nop");
}
//-------------------------------------------------------------------------------------------------
// Enalbe Controller (0-2)
//-------------------------------------------------------------------------------------------------
void GLCD_EnableController(unsigned char controller)
{
switch(controller){
	case 0 : KS0108_CTRL_CLR = KS0108_CS1; break;
	case 1 : KS0108_CTRL_CLR = KS0108_CS2; break;
	case 2 : KS0108_CTRL_CLR = KS0108_CS3; break;
	}
}
//-------------------------------------------------------------------------------------------------
// Disable Controller (0-2)
//-------------------------------------------------------------------------------------------------
void GLCD_DisableController(unsigned char controller)
{
switch(controller){
	case 0 : KS0108_CTRL_SET = KS0108_CS1; break;
	case 1 : KS0108_CTRL_SET = KS0108_CS2; break;
	case 2 : KS0108_CTRL_SET = KS0108_CS3; break;
	}
}
//-------------------------------------------------------------------------------------------------
// Read Status byte from specified controller (0-2)
//-------------------------------------------------------------------------------------------------
unsigned char GLCD_ReadStatus(unsigned char controller)
{
unsigned char status;
KS0108_DATA_INP |= (0xFF << KS0108_D0);
KS0108_CTRL_SET = KS0108_RW;
KS0108_CTRL_CLR = KS0108_RS;
GLCD_EnableController(controller);
KS0108_CTRL_SET = KS0108_EN;
GLCD_Delay();
status = (KS0108_DATA_PIN >> KS0108_D0);
KS0108_CTRL_CLR = KS0108_EN;
GLCD_DisableController(controller);
return status;
}
//-------------------------------------------------------------------------------------------------
// Write command to specified controller
//-------------------------------------------------------------------------------------------------
void GLCD_WriteCommand(unsigned char commandToWrite, unsigned char controller)
{
while(GLCD_ReadStatus(controller)&DISPLAY_STATUS_BUSY);
KS0108_DATA_OUT |= (0xFF << KS0108_D0);
KS0108_CTRL_CLR = (KS0108_RW | KS0108_RS);
GLCD_EnableController(controller);

KS0108_DATA_SET = (commandToWrite << KS0108_D0);
commandToWrite ^= 0xFF;
KS0108_DATA_CLR = (commandToWrite << KS0108_D0);

KS0108_CTRL_SET = KS0108_EN;
GLCD_Delay();
KS0108_CTRL_CLR = KS0108_EN;
GLCD_DisableController(controller);
}
//-------------------------------------------------------------------------------------------------
// Read data from current position
//-------------------------------------------------------------------------------------------------
unsigned char GLCD_ReadData(void)
{
unsigned char data;
while(GLCD_ReadStatus(screen_x / 64)&DISPLAY_STATUS_BUSY);
KS0108_DATA_INP |= (0xFF << KS0108_D0);
KS0108_CTRL_SET = (KS0108_RW | KS0108_RS);
GLCD_EnableController(screen_x / 64);
//GLCD_Delay();
KS0108_CTRL_SET = KS0108_EN;
GLCD_Delay();
data = (KS0108_DATA_PIN >>  KS0108_D0);
KS0108_CTRL_CLR = KS0108_EN;
GLCD_DisableController(screen_x / 64);
screen_x++;
return data;
}
//-------------------------------------------------------------------------------------------------
// Write data to current position
//-------------------------------------------------------------------------------------------------
void GLCD_WriteData(unsigned char dataToWrite)
{
while(GLCD_ReadStatus(screen_x / 64)&DISPLAY_STATUS_BUSY);
KS0108_DATA_OUT |= (0xFF << KS0108_D0);
KS0108_CTRL_CLR = KS0108_RW;
KS0108_CTRL_SET = KS0108_RS;

KS0108_DATA_SET = (dataToWrite << KS0108_D0);
dataToWrite ^= 0xFF;
KS0108_DATA_CLR = (dataToWrite << KS0108_D0);

GLCD_EnableController(screen_x / 64);
KS0108_CTRL_SET = KS0108_EN;
GLCD_Delay();
KS0108_CTRL_CLR = KS0108_EN;
GLCD_DisableController(screen_x / 64);
screen_x++;
}
//-------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------
void GLCD_InitalizePorts(void)
{
Init();
*AT91C_PMC_PCER = (1 << AT91C_ID_PIOA);
KS0108_CTRL_OUT |= (KS0108_CS1 | KS0108_CS2 | KS0108_CS3 | KS0108_RS | KS0108_RW | KS0108_EN);
KS0108_CTRL_SET = (KS0108_CS1 | KS0108_CS2 | KS0108_CS3);
}
//-------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------
unsigned char GLCD_ReadByteFromROMMemory(char * ptr)
{
return *(ptr);
}
//-------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------
