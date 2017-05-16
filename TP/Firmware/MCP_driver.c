#include <xc.h>
#include <sys/attribs.h>
#include "0x2a002a.h"

void dummy_cb(void);
extern u8  SPI2_state;

void    MCP_init()
{
//        u8 GPINTEN[4] = {0x40, 0x04, 0xFF, 0xFF}; // active tous les interrupts on change
//        u8 DEFVAL[4] = {0x40, 0x04, 0xFF, 0xFF}; // active tous les interrupts on change
        u8 GPPU[4] = {0x40, 0x0C, 0xFF, 0xFF};
       // u8 GPIO_BANK_0[4] = {0x41, 0x12, 0x00, 0x00};

        SPI2_write_callback(GPPU, 4, &dummy_cb);
        while (SPI2_state != E_SPI2_DONE);
        //SPI2_write_callback(GPIO_BANK_0, 4, &dummy_cb);
}