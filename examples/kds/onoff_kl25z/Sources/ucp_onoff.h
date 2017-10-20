/**
* @file  ucp_onoff.h
* @brief On-off controller (bang-bang controller with hysteresis)
*
* Hysteresis:
* up_hysteresis - when heating and value is above (setpoint + up), output off
* down_hysteresis - when cooling and value is below (setpoint - down), output on.
* 
*/
#ifndef  UCP_ONOFF_H
    #define  UCP_ONOFF_H

/* Vlastni definice datovych typu, jako podle C99 */
#include "ucp_stdint.h"	/* standard types included in EWL */
#include "ucp_stdbool.h"	/* our own bool type */

/** Chybove kody pro UCP
*/
#define     UCP_ERROR_OK        (0)
#define     UCP_ERROR             (1)

/** Data pro 2-polohovy (on-off) regulator
 * Poznamka: hystereze muze byt zadana v libovolnych jednotkach; regulator
 * zajistuje funkci nasledovne:
 * Pri topeni (y roste): 
 * pro y > (w + up_hysteresis)  -->  vypnout vystup (u = 0)
 * else u = 1 
 * Pro chladnuti (y klesa) 
 * pro y < (w - down_hysteresis) --> zapnout vystup (u = 1)
 * else  u = 0 
 * kde:
 * y - regulovana hodnota
 * w - zadana hodnota     
*/
struct onoff_controller_data
{
    float up_hysteresis;
    float down_hysteresis;          
    uint8_t uk1;        /* last value of control signal - to know our state (heating/cooling) */     
};

typedef struct onoff_controller_data  UCP_ONOFF_REG;


uint8_t ucp_onoff_init(UCP_ONOFF_REG* pReg, float up_hysteresis, float down_hysteresis);
uint8_t ucp_onoff_step(UCP_ONOFF_REG* pReg, float y, float setpoint);



/* ------------- end of file ------------- */
#endif  /* UCP_ONOFF_H */
