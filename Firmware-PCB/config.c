// PIC32MX150F128B Configuration Bit Settings

// 'C' source line config statements

// DEVCFG3
// USERID = No Setting
#pragma config PMDL1WAY = ON            // Peripheral Module Disable Configuration (Allow only one reconfiguration)
#pragma config IOL1WAY = ON             // Peripheral Pin Select Configuration (Allow only one reconfiguration)

// DEVCFG2
#pragma config FPLLIDIV = DIV_12        // PLL Input Divider (12x Divider)
#pragma config FPLLMUL = MUL_24         // PLL Multiplier (24x Multiplier)
#pragma config FPLLODIV = DIV_2         // System PLL Output Clock Divider (PLL Divide by 2)

// DEVCFG1
#pragma config FNOSC = PRI              // Oscillator Selection Bits (Primary Osc (XT,HS,EC))
#pragma config FSOSCEN = OFF            // Secondary Oscillator Enable (Disabled)
#pragma config IESO = ON                // Internal/External Switch Over (Enabled)
#pragma config POSCMOD = XT             // Primary Oscillator Configuration (XT osc mode)
#pragma config OSCIOFNC = OFF           // CLKO Output Signal Active on the OSCO Pin (Disabled)
#pragma config FPBDIV = DIV_1           // Peripheral Clock Divisor (Pb_Clk is Sys_Clk/1)
#pragma config FCKSM = CSDCMD           // Clock Switching and Monitor Selection (Clock Switch Disable, FSCM Disabled)
#pragma config WDTPS = PS1048576        // Watchdog Timer Postscaler (1:1048576)
#pragma config WINDIS = OFF             // Watchdog Timer Window Enable (Watchdog Timer is in Non-Window Mode)
#pragma config FWDTEN = OFF             // Watchdog Timer Enable (WDT Disabled (SWDTEN Bit Controls))
#pragma config FWDTWINSZ = WINSZ_25     // Watchdog Timer Window Size (Window Size is 25%)

// DEVCFG0
#pragma config JTAGEN = OFF             // JTAG Enable (JTAG Disabled)
#pragma config ICESEL = ICS_PGx3        // ICE/ICD Comm Channel Select (Communicate on PGEC3/PGED3)
#pragma config PWP = OFF                // Program Flash Write Protect (Disable)
#pragma config BWP = OFF                // Boot Flash Write Protect bit (Protection Disabled)
#pragma config CP = OFF                 // Code Protect (Protection Disabled)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>

//// PIC32MX150F128B Configuration Bit Settings
//
//// 'C' source line config statements
//
//#include <xc.h>
//
//// DEVCFG3
//// USERID = No Setting
//#pragma config PMDL1WAY = ON            // Peripheral Module Disable Configuration (Allow only one reconfiguration)
//#pragma config IOL1WAY = ON             // Peripheral Pin Select Configuration (Allow only one reconfiguration)
////#pragma config FUSBIDIO = ON            // USB USID Selection (Controlled by the USB Module)
////#pragma config FVBUSONIO = ON           // USB VBUS ON Selection (Controlled by USB Module)
//
//// DEVCFG2
//#pragma config FPLLIDIV = DIV_12        // PLL Input Divider (12x Divider)
//#pragma config FPLLMUL = MUL_24         // PLL Multiplier (24x Multiplier)
//#pragma config FPLLODIV = DIV_256       // System PLL Output Clock Divider (PLL Divide by 256)
//
//// DEVCFG1
//#pragma config FNOSC = PRI              // Oscillator Selection Bits (Primary Osc (XT,HS,EC))
//#pragma config FSOSCEN = OFF            // Secondary Oscillator Enable (Disabled)
//#pragma config IESO = ON                // Internal/External Switch Over (Enabled)
//#pragma config POSCMOD = XT             // Primary Oscillator Configuration (XT osc mode)
//#pragma config OSCIOFNC = OFF           // CLKO Output Signal Active on the OSCO Pin (Disabled)
//#pragma config FPBDIV = DIV_1           // Peripheral Clock Divisor (Pb_Clk is Sys_Clk/1)
//#pragma config FCKSM = CSDCMD           // Clock Switching and Monitor Selection (Clock Switch Disable, FSCM Disabled)
//#pragma config WDTPS = PS1048576        // Watchdog Timer Postscaler (1:1048576)
//#pragma config WINDIS = OFF             // Watchdog Timer Window Enable (Watchdog Timer is in Non-Window Mode)
//#pragma config FWDTEN = OFF             // Watchdog Timer Enable (WDT Disabled (SWDTEN Bit Controls))
//#pragma config FWDTWINSZ = WINSZ_25     // Watchdog Timer Window Size (Window Size is 25%)
//
//// DEVCFG0
//#pragma config JTAGEN = OFF             // JTAG Enable (JTAG Disabled)
//#pragma config ICESEL = ICS_PGx3        // ICE/ICD Comm Channel Select (Communicate on PGEC3/PGED3)
//#pragma config PWP = OFF                // Program Flash Write Protect (Disable)
//#pragma config BWP = OFF                // Boot Flash Write Protect bit (Protection Disabled)
//#pragma config CP = OFF                 // Code Protect (Protection Disabled)

