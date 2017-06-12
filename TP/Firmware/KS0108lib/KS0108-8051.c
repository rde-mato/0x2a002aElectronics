//-------------------------------------------------------------------------------------------------
// Universal KS0108 driver library
//  8051-based MCU low-level driver.
// (c) Rados³aw Kwiecieñ, radek@dxp.pl
//-------------------------------------------------------------------------------------------------
#include <8051.h>

#define KS0108_DATA_PORT	P2
#define KS0108_DATA_DIR		P2
#define KS0108_DATA_PIN		P2

#define KS0108_CTRL_PORT	P3
#define KS0108_CTR_DIR		P3

#define KS0108_RS			(1 << 0)
#define KS0108_RW			(1 << 1)
#define KS0108_EN			(1 << 2)

#define KS0108_CS1			(1 << 4)
#define KS0108_CS2			(1 << 3)
#define KS0108_CS3			(1 << 5)

extern unsigned char screen_x;
extern unsigned char screen_y;

#define DISPLAY_STATUS_BUSY	0x80

//-------------------------------------------------------------------------------------------------
// Delay function
//-------------------------------------------------------------------------------------------------
void GLCD_Delay(void)
{
//_asm("nop");
}
//-------------------------------------------------------------------------------------------------
// Enalbe Controller (0-2)
//-------------------------------------------------------------------------------------------------
void GLCD_EnableController(unsigned char controller)
{
switch(controller){
	case 0 : KS0108_CTRL_PORT &= ~KS0108_CS1; break;
	case 1 : KS0108_CTRL_PORT &= ~KS0108_CS2; break;
	case 2 : KS0108_CTRL_PORT &= ~KS0108_CS3; break;
	}
}
//-------------------------------------------------------------------------------------------------
// Disable Controller (0-2)
//-------------------------------------------------------------------------------------------------
void GLCD_DisableController(unsigned char controller)
{
switch(controller){
	case 0 : KS0108_CTRL_PORT |= KS0108_CS1; break;
	case 1 : KS0108_CTRL_PORT |= KS0108_CS2; break;
	case 2 : KS0108_CTRL_PORT |= KS0108_CS3; break;
	}
}
//-------------------------------------------------------------------------------------------------
// Read Status from specified controller (0-2)
//-------------------------------------------------------------------------------------------------
unsigned char GLCD_ReadStatus(unsigned char controller)
{
unsigned char status;
KS0108_DATA_DIR = 0xFF;
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

//-------------------------------------------------------------------------------------------------
// Write command to specified controller
//-------------------------------------------------------------------------------------------------
void GLCD_WriteCommand(unsigned char commandToWrite, unsigned char controller)
{
while(GLCD_ReadStatus(controller)&DISPLAY_STATUS_BUSY);
KS0108_DATA_DIR = 0x00;
KS0108_CTRL_PORT &= ~(KS0108_RW | KS0108_RS);
GLCD_EnableController(controller);
KS0108_DATA_PORT = commandToWrite;
KS0108_CTRL_PORT |= KS0108_EN;
GLCD_Delay();
KS0108_CTRL_PORT &= ~KS0108_EN;
GLCD_DisableController(controller);
}
//-------------------------------------------------------------------------------------------------
// Read data from current position
//-------------------------------------------------------------------------------------------------
unsigned char GLCD_ReadData(void)
{
unsigned char tmp;
while(GLCD_ReadStatus(screen_x / 64)&DISPLAY_STATUS_BUSY);
KS0108_DATA_DIR = 0xFF;
KS0108_CTRL_PORT |= (KS0108_RW | KS0108_RS);
GLCD_EnableController(screen_x / 64);
KS0108_CTRL_PORT |= KS0108_EN;
GLCD_Delay();
tmp = KS0108_DATA_PIN;
KS0108_CTRL_PORT &= ~KS0108_EN;
GLCD_DisableController(screen_x / 64);
screen_x++;
return tmp;
}
//-------------------------------------------------------------------------------------------------
// Write data to current position
//-------------------------------------------------------------------------------------------------
void GLCD_WriteData(unsigned char dataToWrite)
{
while(GLCD_ReadStatus(screen_x / 64)&DISPLAY_STATUS_BUSY);
KS0108_DATA_DIR = 0x00;
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
//-------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------
void GLCD_InitializePorts(void)
{
//KS0108_CTRL_DIR |= (KS0108_CS1 | KS0108_CS2 | KS0108_CS3 | KS0108_RS | KS0108_RW | KS0108_EN);
KS0108_CTRL_PORT |= (KS0108_CS1 | KS0108_CS2 | KS0108_CS3);
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

