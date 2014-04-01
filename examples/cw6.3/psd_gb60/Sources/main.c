/*
* Diskretni PSD regulator
* Vzorovy program pro regulaci s vyuzitim knihovny UCP knihovny  (UTB control package)
*
* Author: Jan Dolinay
*
* Program obsahuje PSD regulator a akcni zasah realizuje pomoci PWM.
* Perioda PWM se 2 s; perioda vzorkovani take 2 s.
*
* Nutne upravy viz navod knihovny. Strucne:
* pridat do projektu ucp_app.c a .h; ucp_hal.c a .h; ucp_swpwm.c a .h a 
* upravit tyto soubory podle potreby. 
*
* V ucp_app.c je mozno pomoci makra SIMULACE prepinat mezi rezimem se
* simulovanou rizenou soustavou a realnou.
*
* Poznamky:
* - v nastaveni compilatoru je vhodne zmenit enum na 8-bit unsigned misto vychoziho 16-bit signed.
*/

#include <hidef.h> /* for EnableInterrupts macro */
#include "derivative.h" /* include peripheral declarations */

#include "ucp_app.h"
#include "ucp_swpwm.h"
#include "uart_gb60.h"



interrupt void timer_int(void);    // prototyp funkce pro obslouzeni preruseni
// definujeme globalni promennou jako ukazatel na funkci a to na dane misto pameti
// (na adresuvektoru preruseni) a do teto promenne nastavime adresu nasi funkce timer_int
void (*const obsluha)(void) @0xFFEE = timer_int;

/* Kolik volani obsluhy preruseni casovace odpovida 1 periode vzorkovani.
Zde perioda TOF je 20 ms, perioda vzorkovani je 2 s 
*/
#define     TICKS_IN_TV     (100)
//#define TICKS_IN_TV     (5)     /* Pro simulaci, at dlouho necekame na on_sample() */

uint8_t counter;
volatile uint8_t status;

void main(void) {
    char zn;
  
  // Nastavit casovac pro generovani preruseni kazdych 20 ms
  // Pri kazdem preruseni zavolame ucp_pwm_tick(). (perioda PWM = 2 s)
  // Z obsluhy preruseni take budeme volat ucp_app_on_sample() (perioda vzorkovani).
  TPM1SC = 0x4F;        	// source fbus (20 MHz), delicka 128  
  TPM1MOD = 3125;  		// modulo registr pro 20 ms TOF
  counter = 0;
  status = 0;    
   
  /* Inicializace UCP aplikace (ta vola init regulatoru apod.) */
  ucp_app_init();
  
  uart_init(BD9600);
  
  EnableInterrupts; /* enable interrupts */
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
  
  for(;;) {  
  
    // Obsluha preruseni nam pomoci "status" signalizuje, ze uplynula doba
    // pro volani on_sample()
    if ( status == 1 )
    { 
        status = 0;       
        ucp_app_on_sample();       
    }
    __RESET_WATCHDOG(); /* feeds the dog */
  } /* loop forever */
  /* please make sure that you never leave main */
}

/* Obsluha preruseni od casovace 1 */
interrupt void timer_int(void)  
{
    TPM1SC &= 0x7F;  	// nuluj priznak preruseni 
    counter++;
    
    ucp_pwm_tick();
    
    if ( counter == TICKS_IN_TV )
    {
        counter = 0;
        /* Poznamka: neni vhodne zde volat on_sample() primo, protoze funkce muze trvat
        dlouho (vypocty s float) a zpusobovala by vypadek casovani PWM; proto jen nastavime 
        priznak a volani se provede z hlavni smycky */
         //ucp_app_on_sample();  /* NEVHODNE! */
        
        status = 1;       
    }
}

