#include <xc.h>
#include "tp.h"
#include <sys/attribs.h>

/*
 * LOGIQUE GENERALE
 * 32 LEDs a allumer tiennent sur un u32
 * on cree donc un buffer qui tient dans un u32
 *
 * une variable globale tempo donne le rythme et genere un interrupt regulierement
 * a chacun de ces interrupts, le buffer des LEDs est lu et affiche
 * l'affichage declenche lui meme un interrupt qui va remplir a nouveau le buffer
 * question : doit-on signaler que le buffer est pret pour l'envoyer ? systeme de blocage ?
 *
 * le buffer est rempli en OUant les

 */

extern u8 I2C2_state;
extern u8 I2C2_write_buf[10];

extern u8 buttonbuff[16];

extern u8   db_dirty;
extern u8   ks_dirty;
extern u8   read_buf_dirty;


int main(void)
{
    u8 new[16];
    u8 i;

    init_pattern();
    GPIO_init();
    TIMER_init();
    I2C2_init();
    INT_init();
    T2CONbits.ON = 1;

    HT16_init();

    while (42)
    {
        if (read_buf_dirty)
        {
            read_buf_dirty = 0;
            get_new_input(new);
            for (i = 0; i < 16; i++)
            {
                if (new[i] != buttonbuff[i])
                {
                    led_toggle(i);
                    buttonbuff[i] = new[i];
                }
            }
        }
        if (ks_dirty && I2C2_READY) // changer les dirty et compgnie vers un byte de status masque.
            start_read();
        else if (db_dirty && I2C2_READY)
            led_refresh();
        WDTCONbits.WDTCLR = 1; // CLEAR WATCHDOG
    }
    return (0);
}
