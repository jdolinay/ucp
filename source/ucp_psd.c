/**
* @file  ucp_psd.c
* @brief PSD controller (discrete PID controller)
*
*/

//#include <hidef.h> /* for EnableInterrupts macro */
//#include "derivative.h" /* include peripheral declarations */
#include "ucp_psd.h"

/** Init PSD regulatoru
* @param pReg ukazatel na strukturu alokovanou uzivatelem (globalni promenna)
* @return error code; (0 = OK)
*/
uint8_t ucp_psd_init(UCP_PSD_REG* pReg, float q0, float q1, float q2, float q3)
{
    pReg->q0 = q0;
    pReg->q1 = q1;
    pReg->q2 = q2;
    pReg->q3 = q3;
    
    pReg->uk1 = 0.0f;    
    pReg->ek1 = 0.0f;    
    pReg->ek2 = 0.0f;    
    pReg->ek3 = 0.0f;  
    
    return UCP_ERROR_OK;  
    
}

/** Vypocet dalsiho akcniho zasahu. Nutno volat s periodou vzorkovani! 
* @param pReg ukazatel na strukturu alokovanou uzivatelem (globalni promenna)
* @param y aktualni hodnota regulovane veliciny
* @param setpoint pozadovana hodnota
* @param minVal minimalni mozna hodnota akcniho zasahu
* @param maxVal maximalni mozna hodnota akcniho zasahu
* @return aktualni akcni zasah u(k)
*/
float ucp_psd_step(UCP_PSD_REG* pReg, float y, float setpoint,  float minVal, float maxVal)
{
    float uk;
    float ek;
    
    /* Aktualni regulacni odchylka */
    ek = setpoint - y;
    
    /* Vlastni vypocet akcniho zasahu */
    uk = pReg->uk1 + pReg->q0 * pReg->ek1 + pReg->q1 * pReg->ek2 + pReg->q2 * pReg->ek3;
    
    /* Orezani akcniho zasahu */
    if ( uk < minVal )
        uk = minVal;
    else if ( uk > maxVal )
        uk = maxVal;
    
    /* Aktualizace minulych hodnot */
    pReg->uk1 = uk;
    pReg->ek3 = pReg->ek2;
    pReg->ek2 = pReg->ek1;
    pReg->ek1 = ek;
    
    return uk;   
}
