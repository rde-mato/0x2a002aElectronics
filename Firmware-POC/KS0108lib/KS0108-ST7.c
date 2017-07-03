//-------------------------------------------------------------------------------------------------
// KS0108 low-level driver for ST7 MCU (RKitSTM8 compiler)
// http://en.radzio.dxp.pl/ks0108/
// (c) Rados³aw Kwiecieñ, radek@dxp.pl
//-------------------------------------------------------------------------------------------------
#include <ST7LITE4.H>
#include "ks0108.h"
//---------------------------
// Interface configuration
//---------------------------
#define KS0108_DATA_PORT	PBDR
#define KS0108_DATA_DIR		PBDDR
#define KS0108_DATA_PIN		PBDR
#define KS0108_DATA_OR      PBOR

#define KS0108_CTRL_PORT	PCDR
#define KS0108_CTRL_DIR		PCDDR
#define KS0108_CTRL_OR      PCOR

#define KS0108_RS			(1 << 0)
#define KS0108_RW			(1 << 1)
#define KS0108_EN			(1 << 4)

#define KS0108_CS1			(1 << 5)
#define KS0108_CS2			(1 << 6)
#define KS0108_CS3			(1 << 7)
//---------------------------
extern unsigned char screen_x;
extern unsigned char screen_y;
//---------------------------
// Delay function
//---------------------------
void GLCD_Delay(void)
{
//asm("nop");asm("nop");asm("nop");asm("nop");
}
//---------------------------
// Enable Controller (0-2)
//---------------------------
void GLCD_EnableController(unsigned char controller)
{
switch(controller){
	case 0 : KS0108_CTRL_PORT &= ~KS0108_CS1; break;
	case 1 : KS0108_CTRL_PORT &= ~KS0108_CS2; break;
	case 2 : KS0108_CTRL_PORT &= ~KS0108_CS3; break;
	}
}
//---------------------------
// Disable Controller (0-2)
//---------------------------
void GLCD_DisableController(unsigned char controller)
{
switch(controller){
	case 0 : KS0108_CTRL_PORT |= KS0108_CS1; break;
	case 1 : KS0108_CTRL_PORT |= KS0108_CS2; break;
	case 2 : KS0108_CTRL_PORT |= KS0108_CS3; break;
	}
}
//-----------------------------------------------
// Read Status from specified controller (0-2)
//-----------------------------------------------
unsigned char GLCD_ReadStatus(unsigned char controller)
{
unsigned char status;
KS0108_DATA_DIR = 0x00;
KS0108_CTRL_PORT |= KS0108_RW;
KS0108_CTRL_PORT &= ~KS0108_RS;
GLCD_EnableController(controller);
KS0108_CTRL_PORT |= KS0108_EN;
GLCD_Delay();
status = KS0108_DATA_PIN;
KS0108_CTRL_PORT &= ~KS0108_EN;
GLCD_DisableController(controller);
return status;
}
//-----------------------------------------------
// Write command to specified controller
//-----------------------------------------------
void GLCD_WriteCommand(unsigned char commandToWrite, unsigned char controller)
{
while(GLCD_ReadStatus(controller)&DISPLAY_STATUS_BUSY);
KS0108_DATA_DIR = 0xFF;
KS0108_CTRL_PORT &= ~(KS0108_RW | KS0108_RS);
GLCD_EnableController(controller);
KS0108_DATA_PORT = commandToWrite;
KS0108_CTRL_PORT |= KS0108_EN;
GLCD_Delay();
KS0108_CTRL_PORT &= ~KS0108_EN;
GLCD_DisableController(controller);
}
//-----------------------------------------------
// Read data from current position
//-----------------------------------------------
unsigned char GLCD_ReadData(void)
{
unsigned char dat;
while(GLCD_ReadStatus(screen_x / 64)&DISPLAY_STATUS_BUSY);
KS0108_DATA_DIR = 0x00;
KS0108_CTRL_PORT |= (KS0108_RW | KS0108_RS);
GLCD_EnableController(screen_x / 64);
//GLCD_Delay();
KS0108_CTRL_PORT |= KS0108_EN;
GLCD_Delay();
dat = KS0108_DATA_PIN;
KS0108_CTRL_PORT &= ~KS0108_EN;
GLCD_DisableController(screen_x / 64);
screen_x++;
return dat;
}
//-----------------------------------------------
// Write data to current position
//-----------------------------------------------
void GLCD_WriteData(unsigned char dataToWrite)
{
while(GLCD_ReadStatus(screen_x / 64)&DISPLAY_STATUS_BUSY);
KS0108_DATA_DIR = 0xFF;
KS0108_CTRL_PORT &= ~KS0108_RW;
KS0108_CTRL_PORT |= KS0108_RS;
KS0108_DATA_PORT = dataToWrite;
GLCD_EnableController(screen_x / 64);
KS0108_CTRL_PORT |= KS0108_EN;
GLCD_Delay();
KS0108_CTRL_PORT &= ~KS0108_EN;
GLCD_DisableController(screen_x / 64);
screen_x++;
}
//-----------------------------------------------
//
//-----------------------------------------------
void GLCD_InitalizePorts(void)
{
KS0108_CTRL_PORT |= (KS0108_CS1 | KS0108_CS2 | KS0108_CS3);
KS0108_CTRL_DIR |= (KS0108_CS1 | KS0108_CS2 | KS0108_CS3 | KS0108_RS | KS0108_RW | KS0108_EN);
KS0108_CTRL_OR |= (KS0108_CS1 | KS0108_CS2 | KS0108_CS3 | KS0108_RS | KS0108_RW | KS0108_EN);
KS0108_DATA_OR = 0xFF;
}
//-----------------------------------------------
//
//-----------------------------------------------
unsigned char GLCD_ReadByteFromROMMemory(char * ptr)
{
return *(ptr);
}
//-----------------------------------------------
// End of file
//-----------------------------------------------
