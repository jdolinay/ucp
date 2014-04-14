/*
* @file  ucp_hal.h
*
* @brief MCU-specific code provided by the user.
*
* @note Toto jsou funkce, ktere vyuziva UCP aplikace. Tyto funkce
* musi implementovat sam uzivatel knihovny pro svuj konkretni hardware!
* napr. pokud meri teplotu teplomerem s analogovym vystupem, bude ve
* funkci ucphal_read_input() cist hodnotu z AD prevodniku.
*
*/
#ifndef  UCP_HAL_H
    #define  UCP_HAL_H

/* Vlastni definice datovych typu, jako podle C99 */
//#include "stdint.h"	/* standard types included in EWL */
#include "ucp_stdint.h"
#include "ucp_stdbool.h"	/* our own bool type */


/** The interface used by the user application */
uint8_t ucphal_init(void);
float ucphal_read_input(uint8_t channel);
void ucphal_write_output(uint8_t channel, float value);
float ucphal_read_setpoint(uint8_t channel);

/* Volitelna funkce potrebna pri pouziti softwaroveho PWM z ucp_swpwm.c */
void ucphal_write_pin(uint8_t channel, bool value);

/* ------------- end of file ------------- */
#endif         /* UCP_HAL_H */
