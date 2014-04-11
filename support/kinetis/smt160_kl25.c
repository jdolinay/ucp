/**
 * @file stm160_kl25.c
 * @brief driver for temperature sensor SMT 160-30 for Freedom board FRDM-KL25Z
 * 
 * @note
 * For input we can use pins:
 * PTA12 (TMP1_CH0) - digital pin 3 on Arduino
 * PTA13 (TMP1_CH1) - digital pin 8 on Arduino 
 * 
*/

#include "derivative.h"	// jd
#include "smt160_kl25.h"

/** init the timer TPM1 for measuring temperature using input capture
 * 
 */
void smt160_init()
{
	/* Enable Port A clock (pins used for input capture) */
	SIM_SCGC5 |= SIM_SCGC5_PORTA_MASK;
	
	/* Enable clock for timer TMP1 */
	
	/*  Set timer to input capture mode etc. */
	TPM1_SC = TPM_SC_CMOD(1) | TPM_SC_PS(0);	//0x08;	// CMOD = 01, prescaler = 1
	
	TPM1_C0SC = 0b01001100;   // ; inicializace kanalu CH0 casovace TPM1, rezim Input capture, preruseni 
	                                                               // od nabezne a sestupne hrany 
	TPM1_C1SC = 0b01000100;   // inicializace kanalu CH1 casovace TPM1, rezim Input capture, preruseni 
	                                                             // od nabezne hrany
	
	//EnableInterrupts;
}

/** Get current temperature in C x 100
 *  For example: temperature 22.5 C is returned as 2250.
 *  @returns temperature in C x 100 (22.5 C is returned as 2250)
 */
int	 smt160_get_temp()
{
	return 0;
}

/* TMP1 interrupt handler? */
void FTM1_IRQHandler()
{
	
}

