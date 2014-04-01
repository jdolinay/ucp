/*
* @file  ucp_psd.h
* @brief PSD controller (discrete PID controller)
*
*/
#ifndef  UCP_PSD_H
    #define  UCP_PSD_H

/* Vlastni definice datovych typu, jako podle C99 */
#include "stdint.h"

/** Chybove kody pro UCP
*/
#define     UCP_ERROR_OK        (0)
#define     UCP_ERROR             (1)

/** Data pro PSD regulator
*/
struct psd_controller_data
{
    float q0;        /**>koeficient regulatoru Q0 */
    float q1;
    float q2;
    float q3;
    
    float uk1;      /**> minula hodnota akcniho zasahu */
    float ek1;      /**> minula regulacni odchylka, e(k-1) */
    float ek2;  
    float ek3;
            
};

typedef struct psd_controller_data  UCP_PSD_REG;


uint8_t ucp_psd_init(UCP_PSD_REG* pReg, float q0, float q1, float q2, float q3);
float ucp_psd_step(UCP_PSD_REG* pReg, float y, float setpoint,  float minVal, float maxVal);



/* ------------- end of file ------------- */
#endif