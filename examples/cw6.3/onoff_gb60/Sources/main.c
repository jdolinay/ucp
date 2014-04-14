/**
* 2-polohovy regulator
* Vzorovy program pro regulaci s vyuzitim knihovny UCP knihovny  (UTB control package)
*
* Author: Jan Dolinay
*
* Program obsahuje 2-polohovy (on-off) regulator.
* 
* Nutne upravy viz navod knihovny. Strucne:
* pridat do projektu ucp_app.c a .h; ucp_hal.c a .h; a 
* upravit tyto soubory podle potreby. 
*
* Poznamky:
* - v nastaveni compilatoru je vhodne zmenit enum na 8-bit unsigned misto vychoziho 16-bit signed.
*/

#include <hidef.h> /* for EnableInterrupts macro */
#include "derivative.h" /* include peripheral declarations */

#include "ucp_app.h"
#include "uart.h"

void delay(void);

void main(void)
{
    char zn;
    /* Inicializace UCP aplikace (ta vola init regulatoru apod.) */
    ucp_app_init();
  
    uart_init(BD9600);    
    uart_puts("Odeslanim 's' spustite regulaci...");

    while(1)
    {        
        zn = uart_read();
        if ( zn == 's' )
        {
            uart_puts("Startuji...\n");
            break; 
        }
        else
            uart_puts("Neznamy prikaz\n");
    }
  
    for(;;) 
    {    
        ucp_app_on_sample();       
        delay();
    }
    
    /* please make sure that you never leave main */
}      // end of main()

/* Simple busy-wait delay */
void delay(void)
{
    unsigned int i, j;
    for ( j = 0; j < 20; j++ )
    {
        for ( i=0; i< 0xFFFF; i++ )
            __RESET_WATCHDOG();                
    }           
}
