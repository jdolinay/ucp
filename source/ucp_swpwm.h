/*
* @file  ucp_swpwm.h
*
* @brief Software PWM
*
* @note Jendoducha softwarova PWM pro generovani akcniho zasahu.
* uzivatel musi zajistit pravidelne volani funkce ucp_pwm_tick() tak, aby
* tato byla zavolana 100x za periodu PWM. - napr. z obsluhy preruseni od casovace.
*
*/
#ifndef  UCP_SWPWM_H
    #define  UCP_SWPWM_H

/* Vlastni definice datovych typu, jako podle C99 */
#include "ucp_stdint.h"
#include "ucp_stdbool.h"

/** The interface used by the user application */
void ucp_pwm_tick(void);
void ucp_pwm_setduty(uint8_t channel, uint8_t duty);

/* ------------- end of file ------------- */
#endif         /* UCP_SWPWM_H */