/** 
* @file ucp_swpwm.c
* @brief Softwarove PWM.
* @note Obsahuje jednoduchou softwarovou PWM pro realizaci akcniho zasahu pomoci diskretniho pinu
* mikropocitace. Uzivatel musi zajitit:
* - pravidelne volani funkce pwm_tick
* - implementovat funkci ucphal_write_pin v UCP HAL
* Podporuje 2 kanaly. Mozno snadno rozsirit na vice kanalu.
*
*/
#include "ucp_swpwm.h"
#include "ucp_hal.h"

/* interni globalni promenne */
volatile uint8_t g_ucp_duty1;     /** hodnota sirky pulsu PWM pro kanal 1 */
volatile uint8_t g_ucp_duty2;     /** hodnota sirky pulsu PWM pro kanal 2 */

volatile uint8_t g_ucp_counter = 0;  /** citac pro SW PWM generator */


/** Softwarove PW
* Uzivatel musi zajistit volani s takovou periodou, aby 100 volani  = perioda PWM. 
* napr. vola z obsluhy preruseni od casovace.
*/
void ucp_pwm_tick(void)
{                
    if ( g_ucp_counter == 0 )
    {
        /*  Zapnuti pulsi tam kde jsou potreba */
        if (   g_ucp_duty1 >  0 ) 
            ucphal_write_pin(1, true);
        
         if (   g_ucp_duty2 >  0 ) 
            ucphal_write_pin(2, true);
    }
       
                 
    /* Porovnani pro jednotlive kanaly a vypnuti pulsu */
    
    /* kanal 1 */
    if ( ( g_ucp_counter ==  g_ucp_duty1) && (g_ucp_duty1 < 100) )
        ucphal_write_pin(1, false);
    
    /*  kanal 2 */
    if (  (g_ucp_counter ==  g_ucp_duty2) && (g_ucp_duty2 < 100) )
        ucphal_write_pin(2, false);        
   
   
    g_ucp_counter++;   
    if ( g_ucp_counter == 100 )
        g_ucp_counter = 0;
}


/** Nastaveni sirky pulsu pro dany kanal
* @param channel cislo kanalu
* @param duty hodnota sirky pulsu v rozsahu 0 az 100
* @note . 
*/
void ucp_pwm_setduty(uint8_t channel, uint8_t duty)
{
    //DisableInterrupts;
    switch ( channel )
    {
        case 1:
            g_ucp_duty1 = duty;  
            break;
        case 2:
            g_ucp_duty2 = duty;
        
    }    
    //EnableIUnterrupts;
}