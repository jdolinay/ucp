// --------------------------------------------------------------------------------
// Ovladac tepelne soustavy  v jazyku C:  top_kl25z.C
// Pro Freedom board FRDM-KL25Z (Kinetis MCU)
//
// Pouziva pin C9 pro ovladani topeni.
// Snimani vyuziva ovladac pro SMT160, ktery pouziva piny A12 a A13
// Tedy HW propojeni:
// Pin od snimace na A12 a A13 soucasne.
// Pin pro topeni na C9.
//
// 17-4-2014, Jan Dolinay 
// --------------------------------------------------------------------------------
#include "derivative.h"	
#include "smt160_kl25.h"    /* Ovladac pro snimac teploty SMT160-30 */
#include "top_kl25.h"

/**
 * Inicializace ovladace, pinu apod.
 */
void InitTop(void)
{
    /* Inicializace ovladace snimace teploty */
    smt160_init();
    
    /* Inicializace pinu pouziteho pro ovladani topeni jako vystup */
    /* Enable Port C clock (pin used for output ) */
    SIM_SCGC5 |= SIM_SCGC5_PORTC_MASK;
    GPIOC_PDDR |= (1 << 9);
    PORTC_PCR9 = PORT_PCR_MUX(1);   /* C9 set to function Alt1 (GPIO)*/		
}

/**
 * Namerena teplota soustavy
 * @return teplota ve stupnich C x 100; napr. 23.51 C vrati jako 2351  
 */
short GetTemp(void)
{
    return smt160_get_temp();
}

/**
 * Zapne topeni 
 */
void TopOn(void)
{
    GPIOC_PDOR |= (1 << 9);
}

/**
 * Vypne topeni 
 */
void TopOff(void)
{
    GPIOC_PDOR &= ~(1 << 9);
}


